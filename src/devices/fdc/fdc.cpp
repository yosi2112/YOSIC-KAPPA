#include "fdc.hpp"
#include <algorithm>
#include <limits>
namespace yosic {
namespace {
unsigned command_length(std::uint8_t c) {
    switch (c & 31) {
    case 3:
    case 15:
        return 3;
    case 4:
    case 7:
        return 2;
    case 8:
        return 1;
    case 10:
        return (c & 0xa0) ? 0 : 2;
    case 13:
        return (c & 0xa0) ? 0 : 6;
    case 2:
        return (c & 0x80) ? 0 : 9;
    case 5:
    case 9:
        return (c & 0x20) ? 0 : 9;
    case 6:
    case 12:
    case 17:
    case 25:
    case 29:
        return 9;
    default:
        return 0;
    }
}
[[noreturn]] void protocol() {
    throw Fault("fdc-handshake", FdcBoard::base + 4);
}
} // namespace
Fdc::Fdc(Clock &clock) : clock_(clock) {
    later(256000, [this] { poll(); });
}
bool Fdc::can_change_media(unsigned unit) const {
    return unit < drives.size() && phase_ == Phase::idle && !busy_[unit] &&
           !(media_locks_ & (1u << unit)) &&
           drives[unit].media_generation != std::numeric_limits<std::uint32_t>::max();
}
void Fdc::change_media(unsigned unit, std::shared_ptr<media::Disk> disk, bool read_only,
                       bool double_sided) {
    if (!can_change_media(unit))
        throw std::runtime_error("drive busy or locked; finish the guest disk operation first");
    auto &drive = drives[unit];
    drive.disk = std::move(disk);
    drive.read_only = read_only;
    drive.double_sided = double_sided;
    ++drive.media_generation;
    ++unload_epoch_[unit];
    loaded_[unit] = false;
    ready_[unit] = drive.ready();
    interrupt(static_cast<std::uint8_t>(0xc0 | unit | (ready_[unit] ? 0 : 8)), unit);
}
void Fdc::media_locks(std::uint32_t value) {
    if (value & ~15u)
        throw Fault("invalid-media-lock", FdcBoard::base + 20);
    media_locks_ = value;
}
void Fdc::later(std::uint64_t delay, std::function<void()> action) {
    if (delay > std::numeric_limits<std::uint64_t>::max() - clock_.now())
        throw std::overflow_error("FDC clock overflow");
    std::weak_ptr<int> weak = life_;
    clock_.schedule_at(clock_.now() + delay, [weak, action = std::move(action)] {
        if (!weak.expired())
            action();
    });
}
void Fdc::event(std::uint64_t delay, std::function<void()> action) {
    auto epoch = epoch_;
    later(delay, [this, epoch, action = std::move(action)] {
        if (epoch == epoch_ && !reset_)
            action();
    });
}
void Fdc::interrupt(std::uint8_t status, unsigned unit) {
    // Each drive has one pending interrupt latch. Preserve the order of distinct drives.
    auto i = std::find_if(interrupts_.begin(), interrupts_.end(),
                          [unit](const auto &v) { return (v.status & 3u) == unit; });
    if (i != interrupts_.end())
        *i = {status, pcn_[unit]};
    else
        interrupts_.push_back({status, pcn_[unit]});
}
void Fdc::poll() {
    if (!reset_ && (phase_ == Phase::idle || phase_ == Phase::command)) {
        auto u = poll_unit_;
        bool r = drives[u].ready();
        if (r != ready_[u]) {
            ready_[u] = r;
            interrupt(static_cast<std::uint8_t>(0xc0 | u | (r ? 0 : 8)), u);
        }
        poll_unit_ = (poll_unit_ + 1) % 4;
    }
    later(256000, [this] { poll(); });
}
Signals Fdc::signals() const {
    return {result_irq_ || !interrupts_.empty() || (ndma() && request_), drq_};
}
std::uint32_t Fdc::signal_bits() const {
    auto s = signals();
    std::uint32_t v = unsigned(s.irq) | (unsigned(s.drq) << 1);
    if (std::any_of(loaded_.begin(), loaded_.end(), [](bool b) { return b; }))
        v |= 4;
    for (unsigned u = 0; u < 4; ++u) {
        if (drives[u].ready())
            v |= 1u << (4 + u);
        if (drives[u].index(clock_.now()))
            v |= 1u << (8 + u);
    }
    return v;
}
std::uint8_t Fdc::msr() const {
    if (reset_)
        return 0;
    unsigned v = 0;
    for (unsigned u = 0; u < 4; ++u)
        if (busy_[u])
            v |= 1u << u;
    if (phase_ != Phase::idle)
        v |= 0x10;
    if (phase_ == Phase::execution) {
        if (ndma())
            v |= 0x20;
        if (reading_)
            v |= 0x40;
        if (request_ && ndma())
            v |= 0x80;
    } else {
        v |= 0x80;
        if (phase_ == Phase::result)
            v |= 0x40;
    }
    return static_cast<std::uint8_t>(v);
}
std::uint8_t Fdc::peek_data() const {
    return phase_ == Phase::result && !result_.empty() ? result_.front() : latch_;
}
std::uint8_t Fdc::data(bool dack) {
    if (reset_)
        protocol();
    if (dack && drq_ && phase_ == Phase::idle) {
        drq_ = false;
        return latch_;
    }
    if (phase_ == Phase::result) {
        if (dack && !drq_)
            protocol();
        if (dack)
            drq_ = false;
        result_irq_ = false;
        auto v = result_.front();
        result_.pop_front();
        if (result_.empty())
            phase_ = Phase::idle;
        return v;
    }
    if (phase_ != Phase::execution || !reading_ || !request_ || dack == ndma())
        protocol();
    auto v = latch_;
    accept_byte(0);
    return v;
}
void Fdc::data(std::uint8_t value, bool dack) {
    if (reset_)
        protocol();
    if (phase_ == Phase::execution) {
        if (reading_ || !request_ || dack == ndma())
            protocol();
        accept_byte(value);
        return;
    }
    if (dack || phase_ == Phase::result)
        protocol();
    if (phase_ == Phase::idle) {
        command_.clear();
        command_.push_back(value);
        op_ = value & 31;
        if (!command_length(value)) {
            result({0x80}, false);
            return;
        }
        phase_ = Phase::command;
    } else
        command_.push_back(value);
    if (command_.size() == command_length(command_[0]))
        execute();
}
void Fdc::result(std::initializer_list<std::uint8_t> values, bool irq) {
    ++epoch_;
    ++byte_epoch_;
    result_ = values;
    phase_ = Phase::result;
    request_ = false;
    result_irq_ = irq;
}
void Fdc::unload() {
    auto u = unit_;
    auto epoch = ++unload_epoch_[u];
    auto count = specify1_ & 15;
    later(std::uint64_t(count ? count : 16) * 16000000 * (slow_ ? 2 : 1), [this, u, epoch] {
        if (epoch == unload_epoch_[u])
            loaded_[u] = false;
    });
}
void Fdc::finish(std::uint8_t s1, std::uint8_t s2, std::uint8_t s0) {
    st1_ |= s1;
    st2_ |= s2;
    unsigned status = unit_ | (head_ << 2) | s0;
    if (st1_ || (st2_ & 0x77) || (s0 & 0x18))
        status |= 0x40;
    result({static_cast<std::uint8_t>(status), st1_, st2_, id_[0], id_[1], id_[2], id_[3]}, true);
    unload();
}
void Fdc::reset(bool held) {
    reset_ = held;
    ++epoch_;
    ++byte_epoch_;
    command_.clear();
    result_.clear();
    interrupts_.clear();
    phase_ = Phase::idle;
    drq_ = request_ = result_irq_ = false;
    loaded_.fill(false);
    busy_.fill(false);
    for (unsigned u = 0; u < 4; ++u) {
        ++seek_epoch_[u];
        ++unload_epoch_[u];
        ready_[u] = false;
        pcn_[u] = 0;
    }
    // SPECIFY survives RESET. Ready polling resumes upon release.
}
void Fdc::control(std::uint32_t value) {
    slow_ = (value & 0x100) != 0;
    for (unsigned u = 0; u < 4; ++u)
        drives[u].set_motor((value & (0x10u << u)) != 0, clock_.now());
    bool held = (value & 1) != 0;
    if (held != reset_)
        reset(held);
    if (!reset_ && (value & 2))
        terminal_count();
}
bool Fdc::ready() {
    auto &d = drives[unit_];
    if (!d.ready() || (head_ && !d.double_sided)) {
        finish(0, 0, media_ready_ ? 0x88 : 8);
        return false;
    }
    if (d.fault && writing()) {
        finish(0, 0, 0x10);
        return false;
    }
    if (writing() && d.protected_media()) {
        finish(2);
        return false;
    }
    return true;
}
void Fdc::execute() {
    if (op_ == 3) {
        specify1_ = command_[1];
        specify2_ = command_[2];
        phase_ = Phase::idle;
        return;
    }
    if (op_ == 8) {
        if (interrupts_.empty()) {
            result({0x80}, false);
            return;
        }
        auto i = interrupts_.front();
        interrupts_.pop_front();
        busy_[i.status & 3] = false;
        result({i.status, i.cylinder}, false);
        return;
    }
    unit_ = command_[1] & 3;
    head_ = (command_[1] >> 2) & 1;
    if (op_ == 4) {
        const auto &d = drives[unit_];
        unsigned s = command_[1] & 7;
        if (d.double_sided)
            s |= 8;
        if (d.cylinder == 0)
            s |= 16;
        if (d.ready())
            s |= 32;
        if (d.protected_media())
            s |= 64;
        if (d.fault)
            s |= 128;
        result({static_cast<std::uint8_t>(s)}, false);
        return;
    }
    if (op_ == 7 || op_ == 15) {
        phase_ = Phase::idle;
        busy_[unit_] = true;
        ++seek_epoch_[unit_];
        seek(unit_, head_, op_ == 7 ? 0 : command_[2], op_ == 7);
        return;
    }
    if (std::any_of(busy_.begin(), busy_.end(), [](bool b) { return b; })) {
        result({0x80}, false);
        return;
    }
    st1_ = st2_ = 0;
    tc_ = marked_ = track_match_ = media_ready_ = false;
    track_count_ = 0;
    format_count_ = 0;
    format_id_.clear();
    offset_ = transfer_ = 0;
    reading_ = op_ == 2 || op_ == 6 || op_ == 10 || op_ == 12;
    request_ = drq_ = false;
    id_ = {0, 0, 0, 0};
    if (command_.size() == 9)
        std::copy_n(command_.begin() + 2, 4, id_.begin());
    phase_ = Phase::execution;
    ++epoch_;
    ++unload_epoch_[unit_];
    if (!ready())
        return;
    if ((op_ != 10 && op_ != 13 && id_[3] > 6) ||
        (op_ == 13 && (command_[2] > 6 || !command_[3])) ||
        (scan() && command_[8] != 1 && command_[8] != 2)) {
        finish(4);
        return;
    }
    media_ready_ = true;
    auto delay = loaded_[unit_] ? 0ull : std::uint64_t(specify2_ >> 1) * 2000000 * (slow_ ? 2 : 1);
    loaded_[unit_] = true;
    event(delay, [this] { start_media(); });
}
void Fdc::seek(unsigned u, unsigned h, unsigned target, bool recal, unsigned steps) {
    const auto generation = seek_epoch_[u];
    auto complete = [this, u, h](unsigned flags) {
        interrupt(static_cast<std::uint8_t>(0x20 | u | (h << 2) | flags), u);
    };
    auto &d = drives[u];
    if (!d.ready()) {
        complete(0x48);
        return;
    }
    if ((recal && d.cylinder == 0) || (!recal && pcn_[u] == target)) {
        if (recal)
            pcn_[u] = 0;
        complete(0);
        return;
    }
    if (recal && steps == 77) {
        complete(0x50);
        return;
    }
    later(std::uint64_t(16 - (specify1_ >> 4)) * 1000000 * (slow_ ? 2 : 1),
          [this, u, h, target, recal, steps, generation] {
              if (generation != seek_epoch_[u] || reset_)
                  return;
              auto &drive = drives[u];
              if (drive.ready()) {
                  if (recal || pcn_[u] > target) {
                      if (drive.cylinder)
                          --drive.cylinder;
                      if (pcn_[u])
                          --pcn_[u];
                  } else {
                      if (drive.cylinder < drive.max_cylinder)
                          ++drive.cylinder;
                      ++pcn_[u];
                  }
              }
              seek(u, h, target, recal, steps + 1);
          });
}
std::uint64_t Fdc::byte_time() const {
    return std::uint64_t((command_[0] & 0x40) ? 16000 : 32000) * (slow_ ? 2 : 1);
}
media::Sector &Fdc::sector() {
    return drives[unit_].track(head_)->sectors.at(sector_);
}
void Fdc::start_media() {
    if (!ready())
        return;
    if (op_ == 13 || op_ == 2) {
        const auto &d = drives[unit_];
        auto *t = d.track(head_);
        auto rev = t ? t->revolution : 200000000u;
        auto delay = rev - d.phase(clock_.now(), head_);
        event(delay, [this] {
            if (op_ == 13)
                format_begin();
            else
                find_sector();
        });
    } else
        find_sector();
}
void Fdc::find_sector() {
    if (!ready())
        return;
    auto &d = drives[unit_];
    auto *t = d.track(head_);
    const auto rev = t ? t->revolution : 200000000u;
    const auto timeout = std::uint64_t(rev) * 2 - d.phase(clock_.now(), head_);
    std::uint64_t best = timeout;
    bool found = false, any_id = false;
    const auto rate = t && t->rate ? t->rate : (d.disk->type == 0x20 ? 500000u : 250000u);
    if (t && rate == (slow_ ? 250000u : 500000u))
        for (std::size_t i = 0; i < t->sectors.size(); ++i) {
            const auto &s = t->sectors[i];
            if (!s.id_mark || s.density != ((command_[0] & 0x40) ? 0 : 0x40))
                continue;
            any_id = true;
            if (op_ != 10 && op_ != 2 &&
                (s.id[1] != id_[1] || s.id[2] != id_[2] || s.id[3] != id_[3]))
                continue;
            auto delay = d.delay(head_, i, clock_.now());
            if (delay < best) {
                best = delay;
                sector_ = i;
                found = true;
            }
        }
    if (!found) {
        event(timeout, [this, any_id] {
            if (ready())
                finish(any_id ? 4 : op_ == 10 ? 5 : 1);
        });
        return;
    }
    event(best, [this] { found_sector(); });
}
void Fdc::found_sector() {
    if (!ready())
        return;
    auto &s = sector();
    if (s.status != 0 && s.status != 0x10 && s.status != 0xa0 && s.status != 0xb0 &&
        s.status != 0xe0 && s.status != 0xf0)
        throw std::runtime_error("unsupported D88 sector status; cannot infer FDC result");
    if (!s.id_crc) {
        if (op_ == 2)
            st1_ |= 0x20;
        else {
            id_ = s.id;
            finish(0x20);
            return;
        }
    }
    if (op_ == 10) {
        id_ = s.id;
        event(byte_time() * 6, [this] { finish(); });
        return;
    }
    if (op_ != 2 && s.id[0] != id_[0]) {
        id_ = s.id;
        finish(4, static_cast<std::uint8_t>(0x10 | (s.id[0] == 0xff ? 2 : 0)));
        return;
    }
    if (op_ == 2) {
        track_match_ |= std::equal(s.id.begin(), s.id.end(), command_.begin() + 2);
        id_ = s.id;
        ++track_count_;
    }
    if (!writing() && !s.data_mark) {
        finish(1, 1);
        return;
    }
    marked_ = !writing() && ((s.deleted != 0) != (op_ == 12));
    if (marked_ && (command_[0] & 0x20) && op_ != 2) {
        if (scan())
            st2_ |= 0x40;
        next_sector();
        return;
    }
    offset_ = 0;
    const auto n = op_ == 2 ? command_[5] : id_[3];
    transfer_ = scan() ? std::size_t{128} << n : (n ? std::size_t{128} << n : command_[8]);
    if (!transfer_)
        transfer_ = 128;
    if (transfer_ > s.data.size()) {
        finish(0x20, 0x20);
        return;
    }
    scan_equal_ = scan_low_ = scan_high_ = true;
    if (writing()) {
        s.data_mark = true;
        s.deleted = op_ == 9 ? 0x10 : 0;
        s.data_crc = false;
        s.status = 0;
        drives[unit_].disk->dirty = true;
    }
    // ID-to-data separation. GPL applies after the data field, not before it.
    event(byte_time() * ((command_[0] & 0x40) ? 44 : 24), [this] { request_byte(); });
}
void Fdc::request_byte() {
    if (!ready())
        return;
    if (op_ == 13 && format_id_.empty()) {
        media::Sector s;
        s.density = (command_[0] & 0x40) ? 0 : 0x40;
        s.id_mark = s.id_crc = s.data_mark = s.data_crc = false;
        s.position = static_cast<std::uint32_t>(clock_.now() - format_start_);
        auto *t = drives[unit_].track(head_);
        t->sectors.push_back(std::move(s));
        sector_ = t->sectors.size() - 1;
    }
    request_ = true;
    drq_ = !ndma();
    if (reading_)
        latch_ = sector().data[offset_];
    const auto generation = ++byte_epoch_;
    // NEC service budget: 13 us MFM / 27 us FM; byte cadence remains 16/32 us.
    const auto overhead = writing() ? 1000u : (command_[0] & 0x40) ? 3000u : 5000u;
    const auto budget = byte_time() - std::uint64_t(overhead) * (slow_ ? 2 : 1);
    next_byte_ = clock_.now() + byte_time();
    event(budget, [this, generation] {
        if (byte_epoch_ == generation && request_) {
            const bool last = !scan() && (op_ == 13 ? offset_ % 4 == 3 : offset_ + 1 == transfer_);
            finish(last ? 0 : 0x10, 0, 0x40);
        }
    });
}
void Fdc::accept_byte(std::uint8_t value) {
    request_ = false;
    drq_ = false;
    ++byte_epoch_;
    if (op_ == 13) {
        sector().id[format_id_.size()] = value;
        format_id_.push_back(value);
        ++offset_;
        if (format_id_.size() == 4) {
            format_record();
            return;
        }
    } else if (writing()) {
        auto &s = sector();
        s.data[offset_] = value;
        drives[unit_].disk->dirty = true;
    } else if (scan()) {
        auto disk = sector().data[offset_];
        scan_equal_ &= disk == value;
        scan_low_ &= disk <= value;
        scan_high_ &= disk >= value;
    }
    if (op_ != 13)
        ++offset_;
    const auto delay = next_byte_ - clock_.now();
    event(delay, [this] {
        if (op_ == 13)
            request_byte();
        else if (offset_ == transfer_ || tc_)
            event(byte_time() * (sector().data.size() - offset_ + 2), [this] { end_sector(); });
        else
            request_byte();
    });
}
void Fdc::terminal_count() {
    if (phase_ != Phase::execution)
        return;
    tc_ = true;
    request_ = drq_ = false;
    ++byte_epoch_;
    if (op_ == 13) {
        format_end();
        return;
    }
    if (scan() && transfer_ != 0) {
        const bool hit = op_ == 17 ? scan_equal_ : op_ == 25 ? scan_low_ : scan_high_;
        finish(0, static_cast<std::uint8_t>(hit ? (scan_equal_ ? 8 : 0) : 4));
        return;
    }
    if (op_ == 10 || transfer_ == 0) {
        finish();
        return;
    }
    // Stop host requests now, but finish CRC/write padding on the rotating medium.
    ++epoch_;
    event(byte_time() * (sector().data.size() - offset_ + 2), [this] { end_sector(); });
}
void Fdc::end_sector() {
    if (!ready())
        return;
    auto &s = sector();
    if (writing()) {
        if (tc_ || transfer_ < s.data.size())
            std::fill(s.data.begin() + static_cast<std::ptrdiff_t>(offset_), s.data.end(),
                      std::uint8_t{0});
        s.data_crc = true;
        s.status = 0;
        drives[unit_].disk->dirty = true;
    } else if (!s.data_crc) {
        if (op_ == 2) {
            st1_ |= 0x20;
            st2_ |= 0x20;
        } else {
            finish(0x20, 0x20);
            return;
        }
    }
    if (marked_ && op_ != 2) {
        finish(0, 0x40);
        return;
    }
    if (scan()) {
        const bool hit = op_ == 17 ? scan_equal_ : op_ == 25 ? scan_low_ : scan_high_;
        if (hit) {
            finish(0, scan_equal_ ? 8 : 0);
            return;
        }
        if (tc_) {
            finish(0, 4);
            return;
        }
    } else if (tc_) {
        if (op_ != 2)
            result_id();
        finish();
        return;
    }
    const auto delay = byte_time() * command_[7];
    event(delay, [this] { next_sector(); });
}
void Fdc::result_id() {
    if (id_[2] != command_[6]) {
        ++id_[2];
        return;
    }
    id_[2] = 1;
    if (!(command_[0] & 0x80) || head_ == 1)
        ++id_[0];
    if (command_[0] & 0x80)
        id_[1] ^= 1;
}
void Fdc::next_sector() {
    if (op_ == 2) {
        if (track_count_ >= command_[6]) {
            if (!track_match_)
                st1_ |= 4;
            finish(0x80);
            return;
        }
        find_sector();
        return;
    }
    if (id_[2] == command_[6]) {
        if ((command_[0] & 0x80) && head_ == 0) {
            head_ = 1;
            id_[1] = 1;
            id_[2] = 1;
            find_sector();
            return;
        }
        if (scan())
            finish(0, 4);
        else
            finish(0x80);
        return;
    }
    id_[2] = static_cast<std::uint8_t>(id_[2] + (scan() ? command_[8] : 1));
    find_sector();
}
void Fdc::format_begin() {
    if (!ready())
        return;
    auto *t = drives[unit_].track(head_);
    if (!t) {
        finish(4);
        return;
    }
    t->sectors.clear();
    t->explicit_layout = true;
    t->rate = slow_ ? 250000 : 500000;
    drives[unit_].disk->dirty = true;
    format_start_ = clock_.now();
    transfer_ = std::size_t(command_[3]) * 4;
    offset_ = 0;
    event(t->revolution, [this] { format_end(); });
    event(byte_time() * ((command_[0] & 0x40) ? 162 : 80), [this] { request_byte(); });
}
void Fdc::format_record() {
    auto &s = sector();
    std::copy_n(format_id_.begin(), 4, s.id.begin());
    id_ = s.id;
    s.id_mark = s.id_crc = s.data_mark = true;
    s.data.assign(std::size_t{128} << command_[2], command_[5]);
    s.gap = static_cast<std::uint32_t>(command_[4] * byte_time());
    format_id_.clear();
    ++format_count_;
    const auto delay = next_byte_ - clock_.now() +
                       byte_time() * (s.data.size() + ((command_[0] & 0x40) ? 42 : 22));
    event(delay, [this] {
        if (!ready())
            return;
        sector().data_crc = true;
        if (format_count_ < command_[3])
            event(byte_time() * (command_[4] + ((command_[0] & 0x40) ? 16 : 7)),
                  [this] { request_byte(); });
    });
}
void Fdc::format_end() {
    auto &disk = *drives[unit_].disk;
    auto *t = drives[unit_].track(head_);
    bool canonical = t->sectors.size() == command_[3] && format_id_.empty();
    for (std::size_t i = 0; i < t->sectors.size() && canonical; ++i) {
        const auto &s = t->sectors[i];
        canonical = s.id_mark && s.data_crc &&
                    s.position == media::canonical_position(*t, i, disk.type) &&
                    s.gap == media::canonical_gap(s) * byte_time() && slow_ == (disk.type != 0x20);
    }
    if (canonical) {
        t->explicit_layout = false;
        t->rate = 0;
        for (auto &s : t->sectors) {
            s.position = 0;
            s.gap = 0;
        }
    }
    ++id_[2];
    finish();
}
std::uint32_t FdcBoard::read(std::uint32_t o) {
    if (o == 20 || (o >= 32 && o <= 44 && !(o & 3u)))
        return peek(o);
    if (o == 0)
        return fdc_.msr();
    if (o == 4)
        return fdc_.data();
    if (o == 8)
        return fdc_.signal_bits();
    if (o == 16)
        return fdc_.data(true);
    throw Fault("invalid-mmio-read", base + o);
}
void FdcBoard::write(std::uint32_t o, std::uint32_t v) {
    if (o == 20)
        fdc_.media_locks(v);
    else if (o == 4)
        fdc_.data(static_cast<std::uint8_t>(v));
    else if (o == 12)
        fdc_.control(v);
    else if (o == 16)
        fdc_.data(static_cast<std::uint8_t>(v), true);
    else
        throw Fault("invalid-mmio-write", base + o);
}
std::uint32_t FdcBoard::peek(std::uint32_t o) const {
    if (o == 20)
        return fdc_.media_locks();
    if (o >= 32 && o <= 44 && !(o & 3u))
        return fdc_.drives[(o - 32) / 4].media_generation;
    if (o == 0)
        return fdc_.msr();
    if (o == 4 || o == 16)
        return fdc_.peek_data();
    if (o == 8)
        return fdc_.signal_bits();
    throw Fault("invalid-mmio-peek", base + o);
}
} // namespace yosic

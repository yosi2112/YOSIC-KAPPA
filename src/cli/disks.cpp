#include "disks.hpp"
#include <cctype>
#include <charconv>
#include <cwctype>
#include <set>
namespace yosic {
namespace {
unsigned integer(std::string_view s) {
    unsigned n = 0;
    auto [p, e] = std::from_chars(s.data(), s.data() + s.size(), n);
    if (s.empty() || e != std::errc{} || p != s.data() + s.size())
        throw std::invalid_argument("invalid disk number");
    return n;
}
std::string extension(const std::filesystem::path &p) {
    auto s = p.extension().string();
    for (auto &c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}
bool known(const std::filesystem::path &p) {
    auto e = extension(p);
    return e == ".d88" || e == ".kfd" || e == ".img";
}
std::string upper(std::string s) {
    for (auto &c : s)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}
std::vector<std::string> command_words(std::string_view text) {
    std::vector<std::string> words;
    std::string word;
    bool quoted = false, started = false;
    for (const char c : text) {
        if (c == '"') {
            quoted = !quoted;
            started = true;
        } else if (!quoted && std::isspace(static_cast<unsigned char>(c))) {
            if (started) {
                words.push_back(word);
                word.clear();
                started = false;
            }
        } else {
            word += c;
            started = true;
        }
    }
    if (quoted)
        throw std::invalid_argument("unclosed disk filename quote");
    if (started)
        words.push_back(word);
    return words;
}
unsigned disk_unit(std::string text) {
    text = upper(text);
    if (text.starts_with("FD"))
        text.erase(0, 2);
    if (text.size() != 1 || text[0] < '0' || text[0] > '3')
        throw std::invalid_argument("disk drive must be 0..3");
    return static_cast<unsigned>(text[0] - '0');
}
} // namespace
bool DiskMounts::parse(std::string_view option, const std::function<std::string_view()> &arg) {
    if (option == "--geometry") {
        if (geometry_)
            throw std::invalid_argument("duplicate geometry");
        media::Geometry g;
        g.cylinders = integer(arg());
        g.heads = integer(arg());
        g.sectors = integer(arg());
        g.n = integer(arg());
        (void)media::create(g);
        geometry_ = g;
        return true;
    }
    if (option != "--disk" && option != "--disk-copy" && option != "--disk-index" &&
        option != "--disk-new")
        return false;
    auto unit = integer(arg());
    if (unit >= 4)
        throw std::invalid_argument("disk unit must be 0..3");
    auto &m = mounts_[unit];
    if (option == "--disk") {
        if (m.create || !m.source.empty())
            throw std::invalid_argument("duplicate disk mount");
        m.source = std::string(arg());
        if (m.source.empty())
            throw std::invalid_argument("empty disk path");
    }
    if (option == "--disk-new") {
        if (m.create || !m.source.empty())
            throw std::invalid_argument("duplicate disk mount");
        m.create = true;
    }
    if (option == "--disk-copy") {
        if (!m.output.empty())
            throw std::invalid_argument("duplicate disk output");
        m.output = std::string(arg());
        if (m.output.empty())
            throw std::invalid_argument("empty disk output");
    }
    if (option == "--disk-index") {
        if (m.index)
            throw std::invalid_argument("duplicate disk index");
        m.index = integer(arg());
    }
    return true;
}
void DiskMounts::validate() const {
    std::set<std::wstring> outputs;
    for (const auto &m : mounts_) {
        if (m.source.empty() && !m.create) {
            if (!m.output.empty() || m.index)
                throw std::invalid_argument("disk output/index requires a mount");
            continue;
        }
        if (!m.source.empty() && !known(m.source))
            throw std::invalid_argument("disk extension must be .img, .d88 or .kfd");
        if ((m.create || extension(m.source) == ".img" || extension(m.output) == ".img") &&
            !geometry_)
            throw std::invalid_argument("raw/new disk requires --geometry C H S N");
        if (m.index && extension(m.source) != ".d88")
            throw std::invalid_argument("disk index requires D88");
        if (!m.output.empty()) {
            if (!known(m.output))
                throw std::invalid_argument("unknown disk output format");
            if (std::filesystem::exists(m.output))
                throw std::invalid_argument("disk output exists; choose a new copy path");
            auto normalized = std::filesystem::weakly_canonical(m.output).wstring();
            for (auto &c : normalized)
                c = static_cast<wchar_t>(std::towlower(c));
            if (!outputs.insert(normalized).second)
                throw std::invalid_argument("duplicate disk output path");
        }
    }
}
void DiskMounts::load(Fdc &fdc) {
    for (unsigned u = 0; u < 4; ++u) {
        auto &m = mounts_[u];
        if (m.source.empty() && !m.create)
            continue;
        media::Disk disk;
        if (m.create)
            disk = media::create(*geometry_);
        else {
            auto bytes = media::load(m.source);
            auto e = extension(m.source);
            if (e == ".d88") {
                m.container = media::read_d88(bytes);
                if (m.index.value_or(0) >= m.container.disks.size())
                    throw std::invalid_argument("D88 disk index out of range");
                disk = m.container.disks[m.index.value_or(0)];
            } else if (e == ".kfd")
                disk = media::read_internal(bytes);
            else
                disk = media::read_raw(bytes, *geometry_);
        }
        fdc.drives[u].disk = std::make_shared<media::Disk>(std::move(disk));
        fdc.drives[u].read_only = m.output.empty();
        if (m.create || extension(m.source) == ".img")
            fdc.drives[u].double_sided = geometry_->heads == 2;
    }
}
void DiskMounts::save(const Fdc &fdc) {
    std::array<media::Bytes, 4> outputs;
    // Serialize all drives before creating files, so a lossy conversion writes nothing.
    for (unsigned u = 0; u < 4; ++u) {
        auto &m = mounts_[u];
        if (m.output.empty())
            continue;
        const auto &d = *fdc.drives[u].disk;
        auto e = extension(m.output);
        if (e == ".d88") {
            if (m.container.disks.empty())
                m.container.disks.push_back(d);
            else
                m.container.disks[m.index.value_or(0)] = d;
            outputs[u] = media::write_d88(m.container);
        } else if (e == ".kfd")
            outputs[u] = media::write_internal(d);
        else
            outputs[u] = media::write_raw(d, *geometry_);
    }
    for (unsigned u = 0; u < 4; ++u)
        if (!mounts_[u].output.empty())
            media::save_new(mounts_[u].output, outputs[u]);
}
media::Bytes DiskMounts::serialize(unsigned unit, const Fdc &fdc) const {
    const auto &m = mounts_.at(unit);
    const auto &disk = fdc.drives.at(unit).disk;
    if (!disk)
        throw std::runtime_error("no mounted disk to save");
    const auto e = extension(m.output);
    if (e == ".d88") {
        auto container = m.container;
        if (container.disks.empty())
            container.disks.push_back(*disk);
        else
            container.disks.at(m.index.value_or(0)) = *disk;
        return media::write_d88(container);
    }
    if (e == ".kfd")
        return media::write_internal(*disk);
    return media::write_raw(*disk, *geometry_);
}
void DiskMounts::save_unit(unsigned unit, Fdc &fdc) {
    auto &m = mounts_.at(unit);
    if (!m.output.empty()) {
        const auto bytes = serialize(unit, fdc);
        media::save_new(m.output, bytes);
        m.output.clear();
        fdc.drives[unit].read_only = true;
    } else if (fdc.drives.at(unit).disk && !fdc.drives[unit].read_only &&
               fdc.drives[unit].disk->dirty)
        throw std::runtime_error("unsaved disk changes have no output path");
}
void DiskMounts::attach(Fdc &fdc, unsigned unit, const std::filesystem::path &source) {
    if (!fdc.can_change_media(unit))
        throw std::runtime_error("FDC busy; finish the disk transfer first");
    if (source.empty() || !known(source))
        throw std::invalid_argument("disk extension must be .img, .d88 or .kfd");
    Mount replacement;
    replacement.source = source;
    const auto bytes = media::load(source);
    const auto e = extension(source);
    media::Disk disk;
    const auto geometry = geometry_.value_or(media::Geometry{});
    if (e == ".d88") {
        replacement.container = media::read_d88(bytes);
        if (replacement.container.disks.empty())
            throw std::invalid_argument("empty D88 container");
        disk = replacement.container.disks.front();
    } else if (e == ".kfd")
        disk = media::read_internal(bytes);
    else
        disk = media::read_raw(bytes, geometry);
    auto prepared = std::make_shared<media::Disk>(std::move(disk));
    // Verify and prepare the replacement before saving or changing the old mount.
    save_unit(unit, fdc);
    fdc.change_media(unit, std::move(prepared), true, e != ".img" || geometry.heads == 2);
    mounts_[unit] = std::move(replacement);
}
void DiskMounts::detach(Fdc &fdc, unsigned unit) {
    if (!fdc.can_change_media(unit))
        throw std::runtime_error("FDC busy; finish the disk transfer first");
    if (!fdc.drives[unit].disk)
        return;
    save_unit(unit, fdc);
    fdc.change_media(unit, {}, true, true);
    mounts_[unit] = {};
}
std::string DiskMounts::show(const Fdc &fdc, std::optional<unsigned> unit) const {
    std::string result;
    for (unsigned i = 0; i < mounts_.size(); ++i) {
        if (unit && *unit != i)
            continue;
        result += "FD" + std::to_string(i) + ": ";
        const auto &m = mounts_[i];
        if (!fdc.drives[i].disk)
            result += "not attached";
        else {
            result += m.create ? "new disk" : m.source.string();
            result += fdc.drives[i].protected_media() ? " [read-only]" : " [writable copy]";
            if (!m.output.empty())
                result += " -> " + m.output.string();
        }
        result += "\n";
    }
    return result;
}
std::optional<std::string> DiskMounts::command(std::string_view text, Fdc &fdc) {
    // Only parse disk commands here; SYMDEB expressions/quotes belong to Debugger.
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())))
        text.remove_prefix(1);
    const auto end = text.find_first_of(" \t\r\n");
    const auto op = upper(std::string(text.substr(0, end)));
    if (op != "AT" && op != "DT")
        return {};
    auto args = command_words(text);
    args.erase(args.begin());
    if (op == "AT") {
        if (args.empty())
            return show(fdc);
        if (upper(args[0]) == "-R")
            args.erase(args.begin());
        if (args.size() != 2)
            throw std::invalid_argument("AT [-R] drive filename");
        const auto unit = disk_unit(args[0]);
        attach(fdc, unit, args[1]);
        return "Attached " + show(fdc, unit);
    }
    if (args.size() != 1)
        throw std::invalid_argument("DT drive|ALL");
    if (upper(args[0]) == "ALL") {
        for (unsigned i = 0; i < mounts_.size(); ++i)
            if (!fdc.can_change_media(i))
                throw std::runtime_error("drive busy or locked");
        // Save every old medium before detaching any drive.
        std::array<media::Bytes, 4> prepared;
        for (unsigned i = 0; i < mounts_.size(); ++i) {
            if (!mounts_[i].output.empty())
                prepared[i] = serialize(i, fdc);
            else if (fdc.drives[i].disk && !fdc.drives[i].read_only && fdc.drives[i].disk->dirty)
                throw std::runtime_error("unsaved disk changes have no output path");
        }
        for (unsigned i = 0; i < mounts_.size(); ++i)
            if (!mounts_[i].output.empty()) {
                media::save_new(mounts_[i].output, prepared[i]);
                mounts_[i].output.clear();
                fdc.drives[i].read_only = true;
            }
        for (unsigned i = 0; i < mounts_.size(); ++i)
            detach(fdc, i);
        return show(fdc);
    }
    const auto unit = disk_unit(args[0]);
    detach(fdc, unit);
    return "Detached FD" + std::to_string(unit) + "\n";
}
} // namespace yosic

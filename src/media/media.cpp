#include "media.hpp"
#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>
namespace yosic::media {
namespace {
constexpr std::size_t limit = 64 * 1024 * 1024;
[[noreturn]] void bad(const char *why) {
    throw std::invalid_argument(why);
}
std::uint32_t get(std::span<const std::uint8_t> b, std::size_t p, unsigned n) {
    if (p > b.size() || n > b.size() - p)
        bad("truncated media field");
    std::uint32_t v = 0;
    for (unsigned i = 0; i < n; ++i)
        v |= std::uint32_t(b[p + i]) << (8 * i);
    return v;
}
void put(Bytes &b, std::size_t p, std::uint32_t v, unsigned n) {
    if (p > b.size() || n > b.size() - p)
        bad("media writer bounds");
    for (unsigned i = 0; i < n; ++i)
        b[p + i] = static_cast<std::uint8_t>(v >> (8 * i));
}
std::size_t length(Geometry g) {
    if (!g.cylinders || g.cylinders > 82 || !g.heads || g.heads > 2 || !g.sectors ||
        g.sectors > 255 || g.n > 6)
        bad("invalid raw geometry");
    auto n = std::size_t(g.cylinders) * g.heads * g.sectors * (std::size_t{128} << g.n);
    if (n > limit)
        bad("media too large");
    return n;
}
Bytes disk_bytes(const Disk &d) {
    Bytes b(0x2b0);
    std::copy(d.header.begin(), d.header.end(), b.begin());
    b[26] = d.protect;
    b[27] = d.type;
    for (std::size_t t = 0; t < 164; ++t) {
        const auto &track = d.tracks[t];
        if (track.explicit_layout || track.revolution != 200000000 || track.rate != 0)
            bad("D88 cannot preserve explicit rotational layout");
        if (track.sectors.empty())
            continue;
        if (track.sectors.size() > 4096)
            bad("too many sectors");
        put(b, 32 + 4 * t, static_cast<std::uint32_t>(b.size()), 4);
        for (const auto &s : track.sectors) {
            if (s.data.size() > 65535 || s.position || s.gap)
                bad("D88 cannot preserve sector layout or length");
            const auto p = b.size();
            if (p + 16 + s.data.size() > limit)
                bad("media too large");
            b.resize(p + 16);
            std::copy(s.id.begin(), s.id.end(), b.begin() + static_cast<std::ptrdiff_t>(p));
            put(b, p + 4, static_cast<std::uint32_t>(track.sectors.size()), 2);
            b[p + 6] = s.density;
            b[p + 7] = s.deleted;
            b[p + 8] = encode_status(s);
            std::copy(s.reserved.begin(), s.reserved.end(),
                      b.begin() + static_cast<std::ptrdiff_t>(p + 9));
            put(b, p + 14, static_cast<std::uint32_t>(s.data.size()), 2);
            b.insert(b.end(), s.data.begin(), s.data.end());
        }
    }
    put(b, 28, static_cast<std::uint32_t>(b.size()), 4);
    return b;
}
} // namespace
unsigned canonical_gap(const Sector &s) {
    constexpr unsigned mfm[]{0x1b, 0x36, 0x54, 0x74, 0xff, 0xff, 0xff};
    constexpr unsigned fm[]{0x1b, 0x2a, 0x3a, 0x8a, 0xff, 0xff, 0xff};
    return s.id[3] < 7 ? (s.density == 0x40 ? fm[s.id[3]] : mfm[s.id[3]]) : 0xff;
}
std::uint32_t canonical_position(const Track &t, std::size_t index, std::uint8_t type) {
    if (index >= t.sectors.size())
        throw std::out_of_range("canonical sector index");
    std::uint64_t at = 0, answer = 0;
    for (std::size_t i = 0; i < t.sectors.size(); ++i) {
        const auto &s = t.sectors[i];
        const bool fm = s.density == 0x40;
        const auto byte = std::uint64_t(fm ? 32000 : 16000) * (type == 0x20 ? 1 : 2);
        if (i == 0)
            at = byte * (fm ? 80 : 162);
        if (i == index)
            answer = at;
        at += byte * (s.data.size() + (fm ? 33 : 62) + canonical_gap(s));
    }
    if (at >= t.revolution)
        return static_cast<std::uint32_t>((std::uint64_t(t.revolution) * (index + 1)) /
                                          (t.sectors.size() + 1));
    return static_cast<std::uint32_t>(answer);
}
void decode_status(Sector &s) {
    s.id_mark = s.data_mark = s.id_crc = s.data_crc = true;
    switch (s.status) {
    case 0xa0:
        s.id_crc = false;
        break;
    case 0xb0:
        s.data_crc = false;
        break;
    case 0xe0:
        s.id_mark = false;
        break;
    case 0xf0:
        s.data_mark = false;
        break;
    default:
        break;
    }
}
std::uint8_t encode_status(const Sector &s) {
    auto original = s;
    decode_status(original);
    if (original.id_mark == s.id_mark && original.data_mark == s.data_mark &&
        original.id_crc == s.id_crc && original.data_crc == s.data_crc)
        return s.status;
    const unsigned faults =
        unsigned(!s.id_mark) + unsigned(!s.data_mark) + unsigned(!s.id_crc) + unsigned(!s.data_crc);
    if (faults > 1)
        bad("D88 cannot encode simultaneous sector errors");
    if (!s.id_mark)
        return 0xe0;
    if (!s.data_mark)
        return 0xf0;
    if (!s.id_crc)
        return 0xa0;
    if (!s.data_crc)
        return 0xb0;
    return 0;
}
Disk create(Geometry g, std::uint8_t fill) {
    (void)length(g);
    Disk d;
    d.dirty = true;
    for (unsigned c = 0; c < g.cylinders; ++c)
        for (unsigned h = 0; h < g.heads; ++h)
            for (unsigned r = 1; r <= g.sectors; ++r) {
                Sector s;
                s.id = {static_cast<std::uint8_t>(c), static_cast<std::uint8_t>(h),
                        static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g.n)};
                s.data.assign(std::size_t{128} << g.n, fill);
                d.tracks[c * 2 + h].sectors.push_back(std::move(s));
            }
    return d;
}
Disk read_raw(std::span<const std::uint8_t> b, Geometry g) {
    if (b.size() != length(g))
        bad("raw size does not match explicit geometry");
    auto d = create(g);
    std::size_t at = 0;
    for (auto &t : d.tracks)
        for (auto &s : t.sectors) {
            std::copy_n(b.begin() + static_cast<std::ptrdiff_t>(at), s.data.size(), s.data.begin());
            at += s.data.size();
        }
    return d;
}
Bytes write_raw(const Disk &d, Geometry g) {
    auto canonical = create(g);
    Bytes b;
    b.reserve(length(g));
    if (d.protect || std::any_of(d.header.begin(), d.header.end(), [](auto v) { return v != 0; }) ||
        d.type != 0x20)
        bad("raw cannot preserve disk metadata");
    for (std::size_t i = 0; i < 164; ++i) {
        const auto &t = d.tracks[i];
        const auto &ct = canonical.tracks[i];
        if (t.explicit_layout || t.rate != 0 || t.revolution != ct.revolution ||
            t.sectors.size() != ct.sectors.size())
            bad("raw track mismatch");
        for (std::size_t j = 0; j < t.sectors.size(); ++j) {
            auto s = t.sectors[j];
            const auto data = s.data;
            s.data = ct.sectors[j].data;
            if (s != ct.sectors[j] || data.size() != s.data.size())
                bad("raw would lose sector metadata");
            b.insert(b.end(), data.begin(), data.end());
        }
    }
    return b;
}
D88 read_d88(std::span<const std::uint8_t> all) {
    if (all.empty() || all.size() > limit)
        bad("invalid D88 size");
    D88 result;
    std::size_t base = 0;
    while (base < all.size()) {
        if (result.disks.size() >= 256)
            bad("too many concatenated disks");
        auto b = all.subspan(base);
        const auto size = get(b, 28, 4);
        if (size < 0x2b0 || size > b.size())
            bad("invalid D88 disk size");
        b = b.first(size);
        Disk d;
        std::copy_n(b.begin(), 26, d.header.begin());
        d.protect = b[26];
        d.type = b[27];
        d.original.assign(b.begin(), b.end());
        std::vector<std::pair<std::uint32_t, unsigned>> offsets;
        for (unsigned t = 0; t < 164; ++t) {
            auto p = get(b, 32 + 4 * t, 4);
            if (!p)
                continue;
            if (p < 0x2b0 || p >= size)
                bad("D88 track offset outside disk");
            offsets.emplace_back(p, t);
        }
        std::sort(offsets.begin(), offsets.end());
        for (std::size_t i = 0; i < offsets.size(); ++i) {
            std::size_t p = offsets[i].first;
            const auto end = i + 1 < offsets.size() ? offsets[i + 1].first : size;
            if (p >= end)
                bad("overlapping D88 tracks");
            const auto count = get(b, p + 4, 2);
            if (!count || count > 4096)
                bad("invalid D88 sector count");
            auto &track = d.tracks[offsets[i].second];
            for (unsigned j = 0; j < count; ++j) {
                if (p > end || end - p < 16)
                    bad("truncated D88 sector header");
                if (get(b, p + 4, 2) != count)
                    bad("inconsistent D88 sector count");
                Sector s;
                std::copy_n(b.begin() + static_cast<std::ptrdiff_t>(p), 4, s.id.begin());
                s.density = b[p + 6];
                s.deleted = b[p + 7];
                s.status = b[p + 8];
                std::copy_n(b.begin() + static_cast<std::ptrdiff_t>(p + 9), 5, s.reserved.begin());
                const auto n = get(b, p + 14, 2);
                p += 16;
                if (n > end - p)
                    bad("truncated D88 data");
                s.data.assign(b.begin() + static_cast<std::ptrdiff_t>(p),
                              b.begin() + static_cast<std::ptrdiff_t>(p + n));
                p += n;
                decode_status(s);
                track.sectors.push_back(std::move(s));
            }
            if (p != end)
                bad("D88 track padding/old header unsupported");
        }
        if (!offsets.empty() && offsets.front().first != 0x2b0)
            bad("D88 leading track padding unsupported");
        if (offsets.empty() && size != 0x2b0)
            bad("D88 unreferenced track data");
        result.disks.push_back(std::move(d));
        base += size;
    }
    return result;
}
Bytes write_d88(const D88 &container) {
    if (container.disks.empty())
        bad("empty D88 container");
    Bytes b;
    for (const auto &d : container.disks) {
        const auto data = !d.dirty && !d.original.empty() ? d.original : disk_bytes(d);
        if (data.size() > limit - b.size())
            bad("D88 too large");
        b.insert(b.end(), data.begin(), data.end());
    }
    return b;
}
Bytes write_internal(const Disk &d) {
    Bytes b{'K', 'F', 'D', '2'};
    b.insert(b.end(), d.header.begin(), d.header.end());
    b.push_back(d.protect);
    b.push_back(d.type);
    auto append = [&](std::uint32_t v, unsigned n) {
        auto p = b.size();
        b.resize(p + n);
        put(b, p, v, n);
    };
    for (const auto &t : d.tracks) {
        if (t.sectors.size() > 4096 || t.revolution < 1000000)
            bad("invalid internal track");
        append(t.revolution, 4);
        append(t.explicit_layout, 1);
        if (t.rate != 0 && t.rate != 250000 && t.rate != 500000)
            bad("invalid track rate");
        append(t.rate, 4);
        append(static_cast<std::uint32_t>(t.sectors.size()), 2);
        for (const auto &s : t.sectors) {
            if (s.data.size() > 65535 || s.position >= t.revolution)
                bad("invalid internal sector");
            b.insert(b.end(), s.id.begin(), s.id.end());
            b.push_back(s.density);
            b.push_back(s.deleted);
            b.push_back(s.status);
            b.insert(b.end(), s.reserved.begin(), s.reserved.end());
            append(unsigned(s.id_mark) | (unsigned(s.data_mark) << 1) | (unsigned(s.id_crc) << 2) |
                       (unsigned(s.data_crc) << 3),
                   1);
            append(s.position, 4);
            append(s.gap, 4);
            append(static_cast<std::uint32_t>(s.data.size()), 2);
            b.insert(b.end(), s.data.begin(), s.data.end());
            if (b.size() > limit)
                bad("internal media too large");
        }
    }
    return b;
}
Disk read_internal(std::span<const std::uint8_t> b) {
    if (b.size() < 32 || b.size() > limit ||
        (get(b, 0, 4) != 0x3144464b && get(b, 0, 4) != 0x3244464b))
        bad("invalid internal media signature/version");
    Disk d;
    d.dirty = true;
    std::copy_n(b.begin() + 4, 26, d.header.begin());
    d.protect = b[30];
    d.type = b[31];
    std::size_t p = 32;
    auto take = [&](unsigned n) {
        auto v = get(b, p, n);
        p += n;
        return v;
    };
    for (auto &t : d.tracks) {
        t.revolution = take(4);
        auto explicit_layout = take(1);
        if (explicit_layout > 1 || t.revolution < 1000000)
            bad("invalid internal track");
        t.explicit_layout = explicit_layout != 0;
        if (get(b, 0, 4) == 0x3244464b)
            t.rate = take(4);
        if (t.rate != 0 && t.rate != 250000 && t.rate != 500000)
            bad("invalid track rate");
        auto count = take(2);
        if (count > 4096)
            bad("too many sectors");
        for (unsigned i = 0; i < count; ++i) {
            Sector s;
            for (auto &v : s.id)
                v = static_cast<std::uint8_t>(take(1));
            s.density = static_cast<std::uint8_t>(take(1));
            s.deleted = static_cast<std::uint8_t>(take(1));
            s.status = static_cast<std::uint8_t>(take(1));
            for (auto &v : s.reserved)
                v = static_cast<std::uint8_t>(take(1));
            auto flags = take(1);
            if (flags > 15)
                bad("invalid internal flags");
            s.id_mark = flags & 1;
            s.data_mark = flags & 2;
            s.id_crc = flags & 4;
            s.data_crc = flags & 8;
            s.position = take(4);
            s.gap = take(4);
            auto n = take(2);
            if (p > b.size() || n > b.size() - p || s.position >= t.revolution)
                bad("invalid internal sector");
            s.data.assign(b.begin() + static_cast<std::ptrdiff_t>(p),
                          b.begin() + static_cast<std::ptrdiff_t>(p + n));
            p += n;
            t.sectors.push_back(std::move(s));
        }
    }
    if (p != b.size())
        bad("trailing internal media data");
    return d;
}
Bytes load(const std::filesystem::path &path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f)
        throw std::runtime_error("cannot open disk image");
    auto n = f.tellg();
    if (n < 0 || n > static_cast<std::streamoff>(limit))
        bad("disk image too large");
    Bytes b(static_cast<std::size_t>(n));
    f.seekg(0);
    if (!b.empty() &&
        !f.read(reinterpret_cast<char *>(b.data()), static_cast<std::streamsize>(b.size())))
        throw std::runtime_error("cannot read disk image");
    return b;
}
void save_new(const std::filesystem::path &path, std::span<const std::uint8_t> b) {
    if (std::filesystem::exists(path))
        throw std::runtime_error("disk output already exists");
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char *>(b.data()), static_cast<std::streamsize>(b.size()));
    f.close();
    if (!f)
        throw std::runtime_error("cannot save disk copy");
}
std::uint16_t crc16(std::span<const std::uint8_t> b, std::uint16_t crc) {
    for (auto v : b) {
        crc ^= std::uint16_t(v) << 8;
        for (int i = 0; i < 8; ++i)
            crc = static_cast<std::uint16_t>((crc << 1) ^ ((crc & 0x8000) ? 0x1021 : 0));
    }
    return crc;
}
} // namespace yosic::media

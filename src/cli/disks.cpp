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
} // namespace yosic

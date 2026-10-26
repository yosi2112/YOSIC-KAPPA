#pragma once
#include "devices/fdc/fdc.hpp"
#include <functional>
#include <optional>
#include <string>
#include <string_view>
namespace yosic {
class DiskMounts {
    struct Mount {
        std::filesystem::path source, output;
        std::optional<std::size_t> index;
        bool create = false;
        bool inplace = false;
        media::D88 container;
    };
    std::array<Mount, 4> mounts_;
    std::optional<media::Geometry> geometry_;

  public:
    bool parse(std::string_view option, const std::function<std::string_view()> &arg);
    void validate() const;
    void load(Fdc &fdc);
    void save(const Fdc &fdc);
    std::optional<std::string> command(std::string_view text, Fdc &fdc);
    void attach(Fdc &fdc, unsigned unit, const std::filesystem::path &source,
                bool read_only = false, bool must_exist = false);
    void detach(Fdc &fdc, unsigned unit);
    std::string show(const Fdc &fdc, std::optional<unsigned> unit = {}) const;

  private:
    media::Bytes serialize(unsigned unit, const Fdc &fdc) const;
    void save_unit(unsigned unit, Fdc &fdc);
};
} // namespace yosic

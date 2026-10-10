#pragma once
#include "devices/fdc/fdc.hpp"
#include <functional>
#include <optional>
#include <string_view>
namespace yosic {
class DiskMounts {
    struct Mount {
        std::filesystem::path source, output;
        std::optional<std::size_t> index;
        bool create = false;
        media::D88 container;
    };
    std::array<Mount, 4> mounts_;
    std::optional<media::Geometry> geometry_;

  public:
    bool parse(std::string_view option, const std::function<std::string_view()> &arg);
    void validate() const;
    void load(Fdc &fdc);
    void save(const Fdc &fdc);
};
} // namespace yosic

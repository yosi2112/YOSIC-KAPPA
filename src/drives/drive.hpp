#pragma once
#include "media/media.hpp"
#include <memory>
#include <stdexcept>
namespace yosic {
struct Drive {
    std::shared_ptr<media::Disk> disk;
    bool motor = false, present = true, read_only = true, fault = false, double_sided = true;
    unsigned cylinder = 0, max_cylinder = 81;
    std::uint64_t motor_epoch = 0;
    std::uint32_t media_generation = 0;
    bool ready() const { return disk && motor && present; }
    bool protected_media() const { return read_only || (disk && disk->protect); }
    void set_motor(bool on, std::uint64_t now) {
        if (on && !motor)
            motor_epoch = now;
        motor = on;
    }
    std::uint64_t phase(std::uint64_t now, unsigned head = 0) const {
        const auto period = track(head) ? track(head)->revolution : 200000000u;
        return motor ? (now - motor_epoch) % period : 0;
    }
    bool index(std::uint64_t now) const { return ready() && phase(now) < 2000000; }
    media::Track *track(unsigned head) {
        return disk && cylinder < 82 && head < 2 ? &disk->tracks[cylinder * 2 + head] : nullptr;
    }
    const media::Track *track(unsigned head) const {
        return disk && cylinder < 82 && head < 2 ? &disk->tracks[cylinder * 2 + head] : nullptr;
    }
    std::uint32_t position(unsigned head, std::size_t sector) const {
        const auto *t = track(head);
        if (!t || sector >= t->sectors.size())
            throw std::out_of_range("sector position");
        return t->explicit_layout ? t->sectors[sector].position
                                  : media::canonical_position(*t, sector, disk->type);
    }
    std::uint64_t delay(unsigned head, std::size_t sector, std::uint64_t now) const {
        const auto *t = track(head);
        return (position(head, sector) + std::uint64_t(t->revolution) - phase(now, head)) %
               t->revolution;
    }
};
} // namespace yosic

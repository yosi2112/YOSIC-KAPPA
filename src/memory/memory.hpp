#pragma once
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace yosic {
struct Fault : std::runtime_error {
    std::uint32_t address;
    Fault(const std::string &kind, std::uint32_t at) : std::runtime_error(kind), address(at) {}
};
class Bus {
  public:
    virtual ~Bus() = default;
    virtual std::uint32_t read32(std::uint32_t address) = 0;
    virtual void write32(std::uint32_t address, std::uint32_t value) = 0;
    virtual std::uint32_t fetch32(std::uint32_t address) const = 0;
    virtual std::uint32_t peek32(std::uint32_t address) const = 0;
};
class Memory : public Bus {
  public:
    static constexpr std::uint32_t rom_base = 0x00ff0000;
    Memory() : ram_(0x100000, 0), rom_(0x10000, 0) {}
    void load_rom(std::span<const std::uint8_t> bytes);
    std::uint32_t read32(std::uint32_t address) override { return peek32(address); }
    void write32(std::uint32_t address, std::uint32_t value) override;
    std::uint32_t fetch32(std::uint32_t address) const override { return peek32(address); }
    std::uint32_t peek32(std::uint32_t address) const override;
    static void aligned(std::uint32_t address) {
        if ((address & 3u) != 0)
            throw Fault("unaligned-access", address);
    }

  private:
    std::vector<std::uint8_t> ram_;
    std::vector<std::uint8_t> rom_;
};
} // namespace yosic

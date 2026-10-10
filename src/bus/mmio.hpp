#pragma once
#include "memory/memory.hpp"
#include <vector>
namespace yosic {
struct Signals {
    bool irq = false;
    bool drq = false;
};
class Device {
  public:
    virtual ~Device() = default;
    virtual std::uint32_t read(std::uint32_t offset) = 0;
    virtual void write(std::uint32_t offset, std::uint32_t value) = 0;
    virtual std::uint32_t peek(std::uint32_t offset) const = 0;
    virtual Signals signals() const { return {}; }
};
class MappedBus : public Memory {
    struct Mapping {
        std::uint32_t base;
        std::uint64_t end;
        Device *device;
    };
    std::vector<Mapping> mappings_;
    const Mapping *find(std::uint32_t address) const;

  public:
    void map(std::uint32_t base, std::uint32_t size, Device &device);
    std::uint32_t read32(std::uint32_t address) override;
    void write32(std::uint32_t address, std::uint32_t value) override;
    std::uint32_t peek32(std::uint32_t address) const override;
    std::uint32_t fetch32(std::uint32_t address) const override { return Memory::peek32(address); }
};
} // namespace yosic

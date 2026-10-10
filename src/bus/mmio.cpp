#include "mmio.hpp"
namespace yosic {
void MappedBus::map(std::uint32_t base, std::uint32_t size, Device &device) {
    const std::uint64_t end = std::uint64_t(base) + size;
    if ((base & 3u) || (size & 3u) || size == 0 || end > 0x100000000ull)
        throw std::invalid_argument("invalid MMIO range");
    if (base < 0x100000 || (base < 0x1000000 && end > rom_base))
        throw std::invalid_argument("MMIO overlaps RAM or ROM");
    for (const auto &m : mappings_)
        if (base < m.end && end > m.base)
            throw std::invalid_argument("overlapping MMIO mapping");
    mappings_.push_back({base, end, &device});
}
const MappedBus::Mapping *MappedBus::find(std::uint32_t address) const {
    aligned(address);
    for (const auto &m : mappings_)
        if (address >= m.base && std::uint64_t(address) + 4 <= m.end)
            return &m;
    return nullptr;
}
std::uint32_t MappedBus::read32(std::uint32_t address) {
    if (const auto *m = find(address))
        return m->device->read(address - m->base);
    return Memory::peek32(address);
}
void MappedBus::write32(std::uint32_t address, std::uint32_t value) {
    if (const auto *m = find(address)) {
        m->device->write(address - m->base, value);
        return;
    }
    Memory::write32(address, value);
}
std::uint32_t MappedBus::peek32(std::uint32_t address) const {
    if (const auto *m = find(address))
        return m->device->peek(address - m->base);
    return Memory::peek32(address);
}
} // namespace yosic

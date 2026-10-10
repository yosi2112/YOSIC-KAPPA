#include "memory.hpp"
#include <algorithm>
namespace yosic {
void Memory::load_rom(std::span<const std::uint8_t> bytes) {
    if (bytes.empty() || bytes.size() > rom_.size() || bytes.size() % 4 != 0)
        throw std::invalid_argument("ROM size must be 4..65536 bytes, aligned to 4");
    std::fill(rom_.begin(), rom_.end(), std::uint8_t{0});
    std::copy(bytes.begin(), bytes.end(), rom_.begin());
}
std::uint32_t Memory::peek32(std::uint32_t address) const {
    aligned(address);
    const std::vector<std::uint8_t> *data = nullptr;
    std::uint32_t offset = address;
    if (address <= 0xffffc)
        data = &ram_;
    else if (address >= rom_base && address <= rom_base + 0xfffc) {
        data = &rom_;
        offset -= rom_base;
    } else
        throw Fault("unmapped-access", address);
    std::uint32_t result = 0;
    for (unsigned i = 0; i < 4; ++i)
        result |= std::uint32_t((*data)[offset + i]) << (i * 8);
    return result;
}
void Memory::write32(std::uint32_t address, std::uint32_t value) {
    aligned(address);
    if (address >= rom_base && address <= rom_base + 0xfffc)
        throw Fault("read-only-memory", address);
    if (address > 0xffffc)
        throw Fault("unmapped-access", address);
    for (unsigned i = 0; i < 4; ++i)
        ram_[address + i] = static_cast<std::uint8_t>(value >> (i * 8));
}
} // namespace yosic

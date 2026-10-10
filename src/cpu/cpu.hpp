#pragma once
#include "memory/memory.hpp"
#include <array>
#include <cstdint>
#include <optional>
namespace yosic {
struct State {
    std::array<std::uint32_t, 16> r{};
    std::array<std::uint16_t, 4> segment{0xff, 0, 0, 0};
    std::uint16_t pc = 0;
    std::uint16_t flags = 0;
    bool operator==(const State &) const = default;
};
class Cpu {
  public:
    State state;
    std::optional<std::uint32_t> instruction;
    static std::uint32_t physical(std::uint16_t segment, std::uint32_t offset) {
        return (std::uint32_t(segment) << 16) + offset;
    }
    std::uint32_t address() const { return physical(state.segment[0], state.pc); }
    std::uint32_t reg(unsigned index) const { return index == 15 ? 0 : state.r.at(index); }
    void step(Bus &bus);
};
} // namespace yosic

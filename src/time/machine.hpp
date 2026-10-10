#pragma once
#include "clock.hpp"
#include "cpu/cpu.hpp"
namespace yosic {
class Machine {
  public:
    Cpu cpu;
    Clock clock;
    void step(Bus &bus) {
        if (clock.now() > std::numeric_limits<std::uint64_t>::max() - 1000)
            throw std::overflow_error("virtual time overflow");
        clock.advance(clock.now());
        cpu.step(bus);
        clock.advance(clock.now() + 1000);
    }
};
} // namespace yosic

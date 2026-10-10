#include "cpu.hpp"
namespace yosic {
void Cpu::step(Bus &bus) {
    instruction.reset();
    if ((state.pc & 3u) != 0)
        throw Fault("unaligned-pc", address());
    const auto word = bus.fetch32(address());
    instruction = word;
    const unsigned op = word >> 26, a = (word >> 21) & 31, b = (word >> 16) & 31,
                   c = (word >> 11) & 31;
    const auto illegal = [&] { throw Fault("illegal-instruction", address()); };
    const auto valid_reg = [&](unsigned r) {
        if (r >= 16)
            illegal();
    };
    switch (op) {
    case 0:
        if (word & 0x03ffffffu)
            illegal();
        break;
    case 1:
    case 2:
    case 0x11:
        valid_reg(a);
        valid_reg(b);
        break;
    case 3:
    case 4:
    case 5:
    case 8:
        valid_reg(a);
        if (word & 0x001fffffu)
            illegal();
        break;
    case 6:
        if (word & 0x03e00000u)
            illegal();
        break;
    case 0x10:
    case 0x12:
    case 0x14:
    case 0x15:
    case 0x16:
        valid_reg(a);
        valid_reg(b);
        valid_reg(c);
        if (word & 0x7ffu)
            illegal();
        break;
    case 0x20:
        if (a > 3 || (word & 0xffffu))
            illegal();
        valid_reg(b);
        break;
    default:
        illegal();
    }
    auto next = state;
    next.pc = static_cast<std::uint16_t>(state.pc + 4);
    const auto put = [&](unsigned r, std::uint32_t value) {
        if (r != 15)
            next.r[r] = value;
    };
    const std::uint32_t imm = (word & 0xffffu) | ((word & 0x8000u) ? 0xffff0000u : 0u);
    const auto data_address = [&] { return physical(state.segment[1], reg(b) + imm); };
    const auto flags = [&](std::uint32_t result, bool carry, bool overflow) {
        next.flags = static_cast<std::uint16_t>((result == 0 ? 1 : 0) | (carry ? 2 : 0) |
                                                ((result >> 31) ? 4 : 0) | (overflow ? 8 : 0));
    };
    switch (op) {
    case 0:
        break;
    case 1:
        put(a, bus.read32(data_address()));
        break;
    case 2:
        bus.write32(data_address(), reg(a));
        break;
    case 3: {
        const auto sp = reg(14) - 4;
        bus.write32(physical(state.segment[2], sp), reg(a));
        next.r[14] = sp;
        break;
    }
    case 4:
    case 5:
        if ((reg(a) == 0) == (op == 4))
            next.pc = static_cast<std::uint16_t>(state.pc + 8);
        break;
    case 6: {
        const std::uint32_t delta = ((word & 0x1fffffu) ^ 0x100000u) - 0x100000u;
        next.pc = static_cast<std::uint16_t>(state.pc + delta * 4u);
        break;
    }
    case 8: {
        const auto value = bus.read32(physical(state.segment[2], reg(14)));
        next.r[14] = reg(14) + 4;
        put(a, value);
        break;
    }
    case 0x10:
    case 0x11: {
        const auto x = reg(b), y = op == 0x11 ? imm : reg(c);
        const auto result = x + y;
        put(a, result);
        flags(result, std::uint64_t(x) + y > 0xffffffffull, ((~(x ^ y) & (x ^ result)) >> 31) != 0);
        break;
    }
    case 0x12: {
        const auto x = reg(b), y = reg(c), result = x - y;
        put(a, result);
        flags(result, x >= y, (((x ^ y) & (x ^ result)) >> 31) != 0);
        break;
    }
    case 0x14:
    case 0x15:
    case 0x16: {
        const auto x = reg(b), y = reg(c);
        const auto result = op == 0x14 ? x & y : op == 0x15 ? x | y : x ^ y;
        put(a, result);
        flags(result, false, false);
        break;
    }
    case 0x20:
        next.segment[a] = static_cast<std::uint16_t>(reg(b));
        break;
    default:
        illegal();
    }
    next.r[15] = 0;
    state = next;
}
} // namespace yosic

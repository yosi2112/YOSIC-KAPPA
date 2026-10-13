#include "debugger.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <iomanip>
#include <sstream>

namespace yosic {
namespace {
std::string hex(std::uint32_t n, unsigned width = 8) {
    std::ostringstream out;
    out << std::uppercase << std::hex << std::setfill('0') << std::setw(width) << n;
    return out.str();
}
std::vector<std::string> tokens(std::string_view line) {
    std::vector<std::string> result;
    std::string token;
    for (const char ch : line) {
        if (std::isspace(static_cast<unsigned char>(ch)) || ch == ',' || ch == '=') {
            if (!token.empty()) {
                result.push_back(token);
                token.clear();
            }
            if (ch == '=')
                result.emplace_back("=");
        } else
            token += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    if (!token.empty())
        result.push_back(token);
    return result;
}
std::string location(std::uint32_t at) {
    return hex(at >> 16, 4) + ":" + hex(at & 0xffff, 4);
}
void require(bool condition, const char *message = "Syntax error") {
    if (!condition)
        throw std::invalid_argument(message);
}
} // namespace

Debugger::Debugger(Machine &machine, Bus &bus, Fdc &fdc, std::uint64_t &steps)
    : machine_(machine), bus_(bus), fdc_(fdc), steps_(steps) {}
void Debugger::emit(const std::string &text) const {
    if (output)
        output(text);
}
unsigned Debugger::register_index(std::string_view name) const {
    if (name == "SP")
        return 14;
    if (name == "RZR")
        return 15;
    const std::array<std::string_view, 6> special{"CS", "DS", "SS", "ES", "PC", "F"};
    if (name == "IP")
        return 20;
    if (name == "FLAGS")
        return 21;
    for (unsigned i = 0; i < special.size(); ++i)
        if (name == special[i])
            return 16 + i;
    if (name.starts_with('R')) {
        unsigned n = 0;
        const auto [end, error] = std::from_chars(name.data() + 1, name.data() + name.size(), n);
        if (error == std::errc{} && end == name.data() + name.size() && n < 16)
            return n;
    }
    throw std::invalid_argument("Unknown KAPPA register");
}
std::uint32_t Debugger::register_value(unsigned index) const {
    if (index < 16)
        return machine_.cpu.reg(index);
    if (index < 20)
        return machine_.cpu.state.segment[index - 16];
    return index == 20 ? machine_.cpu.state.pc : machine_.cpu.state.flags;
}
void Debugger::set_register(unsigned index, std::uint32_t n) {
    require(index != 15, "RZR is read-only");
    require(index < 16 || n <= 0xffff, "16-bit register overflow");
    require(index != 20 || (n & 3u) == 0, "PC must be aligned to four bytes");
    if (index < 16)
        machine_.cpu.state.r[index] = n;
    else if (index < 20)
        machine_.cpu.state.segment[index - 16] = static_cast<std::uint16_t>(n);
    else if (index == 20)
        machine_.cpu.state.pc = static_cast<std::uint16_t>(n);
    else
        machine_.cpu.state.flags = static_cast<std::uint16_t>(n);
}
std::uint32_t Debugger::value(std::string_view text) const {
    try {
        if (text == "F")
            throw std::invalid_argument("hexadecimal F");
        return register_value(register_index(text));
    } catch (const std::invalid_argument &) {
    }
    if (text.ends_with('H'))
        text.remove_suffix(1);
    std::uint32_t n = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), n, 16);
    require(!text.empty() && error == std::errc{} && end == text.data() + text.size(),
            "Invalid hexadecimal value");
    return n;
}
std::uint32_t Debugger::address(std::string_view text, std::uint16_t segment) const {
    const auto colon = text.find(':');
    if (colon != std::string_view::npos) {
        const auto s = value(text.substr(0, colon));
        require(s <= 0xffff, "Segment overflow");
        segment = static_cast<std::uint16_t>(s);
        text.remove_prefix(colon + 1);
    }
    const auto offset = value(text);
    require(offset <= 0xffff, "Offset overflow");
    return Cpu::physical(segment, offset);
}
std::uint32_t Debugger::read(std::uint32_t at, unsigned width) const {
    require(std::uint64_t(at) + width <= 0x100000000ull, "Address overflow");
    std::uint32_t n = 0;
    for (unsigned i = 0; i < width; ++i)
        n |= ((bus_.peek32((at + i) & ~3u) >> (((at + i) & 3u) * 8)) & 255u) << (i * 8);
    return n;
}
void Debugger::edit(std::uint32_t at, const std::vector<std::uint32_t> &data, unsigned width) {
    // Restrict debugger writes to RAM. A byte edit must never read/write an MMIO port.
    require(!data.empty() && data.size() <= 4096 &&
                std::uint64_t(at) + data.size() * width <= 0x100000,
            "Edit range must be inside RAM");
    for (const auto n : data)
        require(width == 4 || n < (1u << (width * 8)), "Value overflow");
    for (const auto n : data)
        for (unsigned i = 0; i < width; ++i) {
            const auto aligned = at & ~3u, shift = (at & 3u) * 8;
            const auto old = bus_.peek32(aligned);
            bus_.write32(aligned, (old & ~(255u << shift)) | (((n >> (i * 8)) & 255u) << shift));
            ++at;
        }
}
std::string Debugger::disassemble(std::uint32_t at) const {
    const auto word = bus_.fetch32(at);
    const unsigned op = word >> 26, a = (word >> 21) & 31, b = (word >> 16) & 31,
                   c = (word >> 11) & 31;
    const auto reg = [](unsigned r) {
        return r == 14 ? "SP" : r == 15 ? "RZR" : "R" + std::to_string(r);
    };
    std::string text = "DD " + hex(word) + "H";
    if (op == 0 && !(word & 0x03ffffffu))
        text = "NOP";
    else if ((op == 1 || op == 2 || op == 0x11) && a < 16 && b < 16) {
        const bool negative = (word & 0x8000u) != 0;
        const auto magnitude = negative ? ((~word + 1) & 0xffffu) : (word & 0xffffu);
        const auto imm = std::string(negative ? "-" : "") + hex(magnitude, 4) + "H";
        if (op == 0x11)
            text = "ADI " + reg(a) + "," + reg(b) + "," + imm;
        else
            text = std::string(op == 1 ? "LRM " : "SRM ") + reg(a) + ",[" + reg(b) +
                   (negative ? "" : "+") + imm + "]";
    } else if ((op == 3 || op == 4 || op == 5 || op == 8) && a < 16 && !(word & 0x001fffffu)) {
        text = std::string(op == 3   ? "PSH "
                           : op == 4 ? "SRZ "
                           : op == 5 ? "SNZ "
                                     : "POP ") +
               reg(a);
    } else if (op == 6 && !(word & 0x03e00000u)) {
        const auto delta = ((word & 0x1fffffu) ^ 0x100000u) - 0x100000u;
        text = "JMP " + hex((at + delta * 4u) & 0xffff, 4) + "H";
    } else if ((op == 0x10 || op == 0x12 || op == 0x14 || op == 0x15 || op == 0x16) && a < 16 &&
               b < 16 && c < 16 && !(word & 0x7ffu)) {
        text = std::string(op == 0x10   ? "ADD "
                           : op == 0x12 ? "SUB "
                           : op == 0x14 ? "ANL "
                           : op == 0x15 ? "ORR "
                                        : "XOR ") +
               reg(a) + "," + reg(b) + "," + reg(c);
    } else if (op == 0x20 && a < 4 && b < 16 && !(word & 0xffffu)) {
        constexpr std::array<const char *, 4> segments{"CS", "DS", "SS", "ES"};
        text = "LSEG " + std::string(segments[a]) + "," + reg(b);
    }
    return location(at) + "  " + hex(word) + "  " + text + "\n";
}
std::string Debugger::registers() const {
    std::string out;
    for (unsigned i = 0; i < 16; ++i)
        out +=
            "R" + std::to_string(i) + "=" + hex(machine_.cpu.reg(i)) + (i % 4 == 3 ? "\n" : "  ");
    for (unsigned i = 16; i < 22; ++i) {
        constexpr std::array<const char *, 6> names{"CS", "DS", "SS", "ES", "PC", "FLAGS"};
        out += std::string(names[i - 16]) + "=" + hex(register_value(i), 4) + "  ";
    }
    out += "\n";
    try {
        out += disassemble(machine_.cpu.address());
    } catch (const Fault &) {
        out += "Instruction unavailable\n";
    }
    return out;
}
std::string Debugger::snapshot() const {
    std::ostringstream out;
    out << (running_ ? "RUNNING" : "PAUSED") << "  instructions=" << steps_
        << "  virtual time=" << machine_.clock.now() << " ns\n"
        << registers();
    constexpr std::array<const char *, 4> phases{"idle", "command", "execution", "result"};
    out << "\nFDC: " << phases[static_cast<unsigned>(fdc_.phase())]
        << "  MSR=" << hex(fdc_.msr(), 2) << "  IRQ=" << fdc_.signals().irq
        << "  DRQ=" << fdc_.signals().drq << "\n";
    for (unsigned i = 0; i < fdc_.drives.size(); ++i) {
        const auto &d = fdc_.drives[i];
        out << "Drive " << i << ": media=" << bool(d.disk) << " motor=" << d.motor
            << " ready=" << d.ready() << " write-protect=" << d.protected_media()
            << " cylinder=" << d.cylinder << '\n';
    }
    out << "\nMemory (DD) " << location(watch_) << '\n';
    for (unsigned i = 0; i < 16; ++i) {
        const auto at = std::uint64_t(watch_) + i * 4;
        if (i % 4 == 0 && at <= 0xffffffffull)
            out << location(static_cast<std::uint32_t>(at)) << "  ";
        try {
            require(at <= 0xfffffffcu, "Address overflow");
            out << hex(read(static_cast<std::uint32_t>(at), 4)) << " ";
        } catch (const std::exception &) {
            out << "???????? ";
        }
        if (i % 4 == 3)
            out << '\n';
    }
    return out.str();
}
void Debugger::interrupt() {
    pending_register_.reset();
    pending_edit_.reset();
    running_ = false;
    tracing_ = 0;
    proceeding_ = 0;
    proceed_return_.reset();
    temporary_.clear();
    emit("Break\n" + registers());
}
bool Debugger::advance() {
    if (!running_ || quitting_)
        return false;
    const auto at = machine_.cpu.address();
    if (proceed_return_ && at == *proceed_return_) {
        proceed_return_.reset();
        if (--proceeding_ == 0) {
            running_ = false;
            emit(registers());
            return false;
        }
    }
    if (!tracing_ && !skip_break_) {
        bool hit = std::find(temporary_.begin(), temporary_.end(), at) != temporary_.end();
        for (auto &bp : breaks_)
            if (bp && bp->enabled && bp->address == at) {
                if (bp->passes)
                    --bp->passes;
                else
                    hit = true;
            }
        if (hit) {
            interrupt();
            return false;
        }
    }
    skip_break_ = false;
    if (proceeding_ && !proceed_return_ && (at & 0xffffu) <= 0xffecu) {
        // Recognize the native Call macro: LRM R2,[R12+...]; PSH R2; JMP; DD return.
        const auto first = bus_.fetch32(at);
        if ((first & 0xffff0000u) == 0x044c0000u && bus_.fetch32(at + 4) == 0x0c400000u &&
            (bus_.fetch32(at + 8) & 0xffe00000u) == 0x18000000u)
            proceed_return_ = at + 16;
    }
    machine_.step(bus_);
    ++steps_;
    if (tracing_ && --tracing_ == 0) {
        running_ = false;
        emit(registers());
    }
    if (proceeding_ && !proceed_return_ && --proceeding_ == 0) {
        running_ = false;
        emit(registers());
    }
    return true;
}
void Debugger::command(std::string_view line) {
    try {
        auto args = tokens(line);
        if (pending_register_) {
            const auto reg = *pending_register_;
            pending_register_.reset();
            require(args.size() <= 1);
            if (!args.empty())
                set_register(reg, value(args[0]));
            return;
        }
        if (pending_edit_) {
            const auto at = *pending_edit_;
            pending_edit_.reset();
            std::vector<std::uint32_t> data;
            for (const auto &s : args)
                data.push_back(value(s));
            if (!data.empty())
                edit(at, data, edit_width_);
            return;
        }
        if (args.empty())
            return;
        const auto op = args.front();
        args.erase(args.begin());
        if (op == "?") {
            require(args.size() <= 1);
            if (!args.empty()) {
                const auto n = value(args[0]);
                emit(hex(n) + "H (" + std::to_string(n) + ")\n");
                return;
            }
            emit("R [register [[=] value]]\nD/DB/DW/DD [range]\nE/EB/EW/ED address [list]\n"
                 "U [range]\nG [=address] [breakpoints]\nT/P [=address] [count]\n"
                 "BP[n] address [passcount]\nBC/BD/BE n|*\nBL\nQ\n"
                 "Hexadecimal values; address=segment:offset; range=start end or start L count.\n"
                 "Ctrl+Break pauses. Registers are R0..R15, SP, RZR, CS, DS, SS, ES, PC, F.\n");
        } else if (op == "Q") {
            require(args.empty());
            quitting_ = true;
        } else if (op == "R") {
            require(args.size() <= 3);
            running_ = false;
            tracing_ = 0;
            proceeding_ = 0;
            proceed_return_.reset();
            if (args.empty()) {
                emit(registers());
            } else {
                const auto index = register_index(args[0]);
                if (args.size() == 1) {
                    emit(args[0] + " " + hex(register_value(index)) + "\n:");
                    pending_register_ = index;
                } else {
                    require(args.size() == 2 || args[1] == "=");
                    set_register(index, value(args.back()));
                    emit(registers());
                }
            }
        } else if (op == "G" || op == "T" || op == "P") {
            auto start = machine_.cpu.address();
            if (!args.empty() && args.front() == "=") {
                require(args.size() >= 2);
                start = address(args[1], machine_.cpu.state.segment[0]);
                args.erase(args.begin(), args.begin() + 2);
            }
            require(!(start & 3u), "Unaligned instruction address");
            std::vector<std::uint32_t> points;
            std::uint32_t trace_count = 0;
            if (op == "T" || op == "P") {
                require(args.size() <= 1);
                trace_count = args.empty() ? 1 : value(args[0]);
                require(trace_count && trace_count <= 0xffff, "Trace count must be 1..FFFF");
            } else {
                require(args.size() <= 10);
                for (const auto &s : args) {
                    const auto p = address(s, machine_.cpu.state.segment[0]);
                    require(!(p & 3u), "Unaligned breakpoint");
                    points.push_back(p);
                }
            }
            machine_.cpu.state.segment[0] = static_cast<std::uint16_t>(start >> 16);
            machine_.cpu.state.pc = static_cast<std::uint16_t>(start);
            tracing_ = op == "T" ? trace_count : 0;
            proceeding_ = op == "P" ? trace_count : 0;
            proceed_return_.reset();
            temporary_ = std::move(points);
            skip_break_ = true;
            running_ = true;
        } else if (op.starts_with("BP")) {
            require(op.size() == 2 || (op.size() == 3 && op[2] >= '0' && op[2] <= '9'));
            require(args.size() == 1 || args.size() == 2);
            unsigned index = op.size() == 3 ? static_cast<unsigned>(op[2] - '0') : 0;
            if (op.size() == 2)
                while (index < breaks_.size() && breaks_[index])
                    ++index;
            require(index < breaks_.size(), "Too many breakpoints");
            const auto at = address(args[0], machine_.cpu.state.segment[0]);
            const auto passes = args.size() == 2 ? value(args[1]) : 0;
            require(!(at & 3u) && passes <= 0xffff, "Invalid breakpoint");
            breaks_[index] = Breakpoint{at, passes, true};
        } else if (op == "BL" || op == "BC" || op == "BD" || op == "BE") {
            require(op == "BL" ? args.empty() : args.size() == 1);
            unsigned index = 0;
            if (op != "BL" && args[0] != "*") {
                index = value(args[0]);
                require(index < breaks_.size(), "Bad breakpoint number (0..9)");
            }
            for (unsigned i = 0; i < breaks_.size(); ++i)
                if (breaks_[i] && (op == "BL" || args[0] == "*" || i == index)) {
                    if (op == "BL")
                        emit(std::to_string(i) + (breaks_[i]->enabled ? " e " : " d ") +
                             location(breaks_[i]->address) + " " + hex(breaks_[i]->passes, 4) +
                             "\n");
                    else if (op == "BC")
                        breaks_[i].reset();
                    else
                        breaks_[i]->enabled = op == "BE";
                }
        } else if (op == "D" || op == "DB" || op == "DW" || op == "DD" || op == "U") {
            if (args.size() == 2 && args[1].starts_with('L') && args[1].size() > 1) {
                const auto count = args[1].substr(1);
                args[1] = "L";
                args.push_back(count);
            }
            require(args.size() <= 3);
            const bool code = op == "U";
            const unsigned width = code || op == "DD" ? 4
                                   : op == "DW"       ? 2
                                   : op == "DB"       ? 1
                                                      : dump_width_;
            const auto seg =
                code ? (unassembled_ ? unassemble_segment_ : machine_.cpu.state.segment[0])
                     : (dumped_ ? dump_segment_ : machine_.cpu.state.segment[1]);
            const auto start =
                args.empty() ? (code ? (unassembled_ ? unassemble_next_ : machine_.cpu.address())
                                     : (dumped_ ? dump_next_ : Cpu::physical(seg, 0)))
                             : address(args[0], seg);
            std::uint32_t count = code ? 8 : 128 / width;
            if (args.size() == 3) {
                require(args[1] == "L");
                count = value(args[2]);
            } else if (args.size() == 2) {
                const auto end = address(args[1], static_cast<std::uint16_t>(start >> 16));
                require(end >= start);
                count = (end - start) / width + 1;
            }
            require(count <= 4096 &&
                        std::uint64_t(start) + std::uint64_t(count) * width <= 0x100000000ull,
                    "Range too large");
            require(!code || !(start & 3u), "Unaligned instruction address");
            std::string result;
            for (std::uint32_t i = 0; i < count; ++i) {
                const auto at = start + i * width;
                if (code)
                    result += disassemble(at);
                else {
                    if ((i * width) % 16 == 0)
                        result += location(at) + "  ";
                    result += hex(read(at, width), width * 2) + " ";
                    if (((i + 1) * width) % 16 == 0 || i + 1 == count)
                        result += "\n";
                }
            }
            emit(result);
            if (code) {
                unassemble_next_ = start + count * width;
                unassemble_segment_ = static_cast<std::uint16_t>(start >> 16);
                unassembled_ = true;
            } else {
                watch_ = start;
                dump_next_ = start + count * width;
                dump_segment_ = static_cast<std::uint16_t>(start >> 16);
                dump_width_ = width;
                dumped_ = true;
            }
        } else if (op == "E" || op == "EB" || op == "EW" || op == "ED") {
            require(!args.empty());
            const auto at = address(args[0], machine_.cpu.state.segment[1]);
            const unsigned width = op == "ED" ? 4 : op == "EW" ? 2 : op == "EB" ? 1 : edit_width_;
            std::vector<std::uint32_t> data;
            for (std::size_t i = 1; i < args.size(); ++i)
                data.push_back(value(args[i]));
            if (data.empty()) {
                require(std::uint64_t(at) + width <= 0x100000, "Edit range must be inside RAM");
                emit(location(at) + " " + hex(read(at, width), width * 2) + ".");
                pending_edit_ = at;
            } else
                edit(at, data, width);
            edit_width_ = width;
            running_ = false;
            tracing_ = 0;
            proceeding_ = 0;
            proceed_return_.reset();
        } else
            throw std::invalid_argument(
                "Unsupported SYMDEB command. Use ? for supported commands.");
    } catch (const std::exception &error) {
        emit(std::string("Error: ") + error.what() + "\n");
    }
}
} // namespace yosic

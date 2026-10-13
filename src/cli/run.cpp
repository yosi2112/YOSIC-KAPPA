#include "run.hpp"
#include "devices/console/console.hpp"
#include "disks.hpp"
#include "fdc_trace.hpp"
#include "monitor.hpp"
#include "time/machine.hpp"
#include <charconv>
#include <chrono>
#include <conio.h>
#include <csignal>
#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace yosic {
namespace {
volatile std::sig_atomic_t interrupted = 0;
volatile std::sig_atomic_t monitor_interrupted = 0;
void interrupt_handler(int signal) {
    if (signal == SIGBREAK)
        monitor_interrupted = 1;
    else
        interrupted = 1;
}
std::uint64_t number(std::string_view text) {
    int base = 10;
    if (text.starts_with("0x") || text.starts_with("0X")) {
        text.remove_prefix(2);
        base = 16;
    }
    std::uint64_t value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value, base);
    if (text.empty() || error != std::errc{} || end != text.data() + text.size())
        throw std::invalid_argument("invalid unsigned number");
    return value;
}
std::uint32_t address_number(std::string_view text) {
    const auto value = number(text);
    if (value > 0xffffffffull)
        throw std::invalid_argument("address exceeds 32 bits");
    return static_cast<std::uint32_t>(value);
}
} // namespace
int run_cli(int argc, char *argv[]) {
    std::string rom_path;
    DiskMounts disks;
    std::optional<std::string> input_path;
    std::optional<std::uint64_t> limit;
    bool registers = false, trace = false, trace_fdc = false, interactive = false, monitor = true;
    bool monitor_requested = false;
    std::optional<std::uint32_t> breakpoint, dump;
    std::uint32_t dump_words = 0;
    try {
        for (int i = 1; i < argc; ++i) {
            const std::string_view option(argv[i]);
            const auto arg = [&]() -> std::string_view {
                if (++i >= argc)
                    throw std::invalid_argument("missing option value");
                return argv[i];
            };
            if (disks.parse(option, arg)) {
                continue;
            } else if (option == "--rom") {
                if (!rom_path.empty())
                    throw std::invalid_argument("duplicate --rom");
                rom_path = arg();
            } else if (option == "--input") {
                if (input_path)
                    throw std::invalid_argument("duplicate --input");
                input_path = arg();
            } else if (option == "--interactive")
                interactive = true;
            else if (option == "--no-monitor") {
                monitor = false;
                monitor_requested = true;
            } else if (option == "--monitor") {
                monitor = true;
                monitor_requested = true;
            } else if (option == "--steps")
                limit = number(arg());
            else if (option == "--step")
                limit = 1;
            else if (option == "--break") {
                breakpoint = address_number(arg());
                if ((*breakpoint & 3u) != 0)
                    throw std::invalid_argument("unaligned breakpoint");
            } else if (option == "--registers")
                registers = true;
            else if (option == "--trace-fdc")
                trace_fdc = true;
            else if (option == "--trace")
                trace = true;
            else if (option == "--dump") {
                dump = address_number(arg());
                const auto count = number(arg());
                if ((*dump & 3u) || count > 4096 ||
                    std::uint64_t(*dump) + count * 4 > 0x100000000ull)
                    throw std::invalid_argument("invalid dump range");
                dump_words = static_cast<std::uint32_t>(count);
            } else
                throw std::invalid_argument("unknown run option");
        }
        disks.validate();
        if (rom_path.empty())
            throw std::invalid_argument("--rom is required");
        if (interactive && (input_path || !_isatty(_fileno(stdin))))
            throw std::invalid_argument("--interactive requires a terminal and excludes --input");
        if (monitor && (input_path || !_isatty(_fileno(stdin)))) {
            if (monitor_requested)
                throw std::invalid_argument("--monitor requires a terminal and excludes --input");
            monitor = false;
        }
    } catch (const std::exception &error) {
        std::cerr << "error: " << error.what() << ". Use --help.\n";
        return 2;
    }
    Machine machine;
    auto &cpu = machine.cpu;
    MappedBus memory;
    std::uint64_t count = 0;
    try {
        const int previous_mode = _setmode(_fileno(stdout), _O_BINARY);
        if (previous_mode == -1)
            throw std::runtime_error("cannot set binary console output");
        struct RestoreMode {
            int mode;
            ~RestoreMode() {
                std::cout.flush();
                if (_setmode(_fileno(stdout), mode) == -1)
                    std::cerr << "error: cannot restore console output mode\n";
            }
        } restore_mode{previous_mode};
        Console console([interactive](std::uint8_t byte) {
            std::cout.put(static_cast<char>(byte));
            if (interactive)
                std::cout.flush();
            if (!std::cout)
                throw std::runtime_error("console output failed");
        });
        memory.map(Console::base, 0x100, console);
        Fdc fdc(machine.clock);
        FdcPort fdc_board(fdc, machine.clock, trace_fdc ? &std::cerr : nullptr);
        disks.load(fdc);
        memory.map(FdcBoard::base, 0x100, fdc_board);
        if (input_path) {
            std::ifstream source(*input_path, std::ios::binary | std::ios::ate);
            if (!source)
                throw std::runtime_error("cannot open console input");
            const auto input_size = source.tellg();
            if (input_size < 0 || input_size > 16 * 1024 * 1024)
                throw std::runtime_error("console input exceeds 16 MiB or cannot be sized");
            std::vector<std::uint8_t> data(static_cast<std::size_t>(input_size));
            source.seekg(0);
            if (!data.empty() && !source.read(reinterpret_cast<char *>(data.data()),
                                              static_cast<std::streamsize>(data.size())))
                throw std::runtime_error("cannot read console input");
            console.feed(data);
            console.close_input();
        } else if (!interactive)
            console.close_input();
        std::ifstream input(rom_path, std::ios::binary | std::ios::ate);
        if (!input)
            throw std::runtime_error("cannot open ROM");
        const auto size = input.tellg();
        if (size < 4 || size > 65536 || size % 4 != 0)
            throw std::runtime_error("invalid ROM size");
        std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
        input.seekg(0);
        if (!input.read(reinterpret_cast<char *>(bytes.data()),
                        static_cast<std::streamsize>(bytes.size())))
            throw std::runtime_error("cannot read ROM");
        memory.load_rom(bytes);
        Debugger debugger(machine, memory, fdc, count);
        std::unique_ptr<HardwareMonitor> hardware;
        if (monitor)
            hardware = std::make_unique<HardwareMonitor>(debugger);
        interrupted = 0;
        monitor_interrupted = 0;
        const auto old_handler = std::signal(SIGINT, interrupt_handler);
        const auto old_break_handler = std::signal(SIGBREAK, interrupt_handler);
        struct RestoreSignal {
            decltype(old_handler) previous;
            decltype(old_break_handler) previous_break;
            ~RestoreSignal() {
                if (previous != SIG_ERR)
                    std::signal(SIGINT, previous);
                if (previous_break != SIG_ERR)
                    std::signal(SIGBREAK, previous_break);
            }
        } restore{old_handler, old_break_handler};
        std::string_view reason = "limit";
        auto input_poll = std::chrono::steady_clock::now();
        while (!limit || count < *limit) {
            if (monitor_interrupted) {
                monitor_interrupted = 0;
                if (hardware)
                    hardware->interrupt();
                else {
                    reason = "interrupted";
                    break;
                }
            }
            if (interrupted) {
                reason = "interrupted";
                break;
            }
            // Monitor and keyboard polling are host work, not guest instructions.
            // Batch them to avoid an OS console call for every emulated instruction.
            if ((count & 0xfffu) == 0 || (hardware && !debugger.running())) {
                if (hardware)
                    hardware->pump();
                if (debugger.quitting()) {
                    reason = "monitor-quit";
                    break;
                }
                const auto now = std::chrono::steady_clock::now();
                if (interactive && (!hardware || debugger.accepts_guest_input()) &&
                    !console.input_closed() && now >= input_poll) {
                    input_poll = now + std::chrono::milliseconds(2);
                    while (_kbhit()) {
                        const auto ch = _getch();
                        if (ch == 3) {
                            interrupted = 1;
                            break;
                        }
                        if (ch == 26) {
                            console.close_input();
                            break;
                        }
                        const std::uint8_t byte = static_cast<std::uint8_t>(ch);
                        console.feed(std::span<const std::uint8_t>(&byte, 1));
                    }
                }
            }
            if (interrupted)
                continue;
            if (hardware && !debugger.running()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }
            if (breakpoint && cpu.address() == *breakpoint) {
                reason = "breakpoint";
                break;
            }
            const auto at = cpu.address();
            if (hardware) {
                try {
                    if (!debugger.advance())
                        continue;
                } catch (const Fault &fault) {
                    hardware->interrupt();
                    debugger.output(std::string("Fault: ") + fault.what() +
                                    " address=" + std::to_string(fault.address) + "\n");
                    continue;
                }
            } else {
                machine.step(memory);
                ++count;
            }
            if (trace)
                std::cerr << "trace pc=0x" << std::hex << at << " word=0x" << *cpu.instruction
                          << std::dec << " time_ns=" << machine.clock.now() << '\n';
        }
        std::cerr << "stop=" << reason << " steps=" << count << " pc=0x" << std::hex
                  << cpu.address() << std::dec << " time_ns=" << machine.clock.now() << '\n';
        if (registers) {
            for (unsigned i = 0; i < 16; ++i)
                std::cerr << "R" << std::dec << i << "=0x" << std::hex << cpu.reg(i) << '\n';
            for (unsigned i = 0; i < 4; ++i)
                std::cerr << "SEG" << std::dec << i << "=0x" << std::hex << cpu.state.segment[i]
                          << '\n';
            std::cerr << "PC=0x" << cpu.state.pc << " FLAGS=0x" << cpu.state.flags << std::dec
                      << '\n';
        }
        if (dump)
            for (std::uint32_t i = 0; i < dump_words; ++i) {
                const auto at = *dump + i * 4;
                std::cerr << "mem[0x" << std::hex << at << "]=0x" << memory.peek32(at) << std::dec
                          << '\n';
            }
        std::cout.flush();
        if (!std::cout)
            throw std::runtime_error("console flush failed");
        disks.save(fdc);
        return 0;
    } catch (const Fault &fault) {
        std::cerr << "fault=" << fault.what() << " pc=0x" << std::hex << cpu.address()
                  << " address=0x" << fault.address;
        if (cpu.instruction)
            std::cerr << " word=0x" << *cpu.instruction;
        std::cerr << std::dec << " steps=" << count << '\n';
        return 3;
    } catch (const std::exception &error) {
        std::cerr << "error: " << error.what() << '\n';
        return 3;
    }
}
} // namespace yosic

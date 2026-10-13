#include "cli/debugger.hpp"
#include "devices/console/console.hpp"
#include <iostream>
#include <stdexcept>

namespace {
void check(bool result, const char *message) {
    if (!result)
        throw std::runtime_error(message);
}
} // namespace
int main() {
    try {
        yosic::Machine machine;
        yosic::MappedBus bus;
        yosic::Fdc fdc(machine.clock);
        yosic::FdcBoard board(fdc);
        bus.map(yosic::FdcBoard::base, 0x100, board);
        yosic::Console console([](std::uint8_t) {});
        bus.map(yosic::Console::base, 0x100, console);
        const std::uint8_t input[]{65, 66};
        console.feed(input);
        std::uint64_t steps = 0;
        yosic::Debugger debugger(machine, bus, fdc, steps);
        std::string log;
        debugger.output = [&](const std::string &s) { log += s; };
        auto command = [&](const std::string &s) {
            log.clear();
            debugger.command(s);
        };
        auto execute = [&]() {
            for (unsigned i = 0; i < 100 && debugger.running(); ++i)
                debugger.advance();
            check(!debugger.running(), "debugger did not pause");
        };
        command("r cs=0");
        command("r pc=100");
        command("r r1 ABCDEF01");
        check(machine.cpu.reg(1) == 0xabcdef01, "register edit");
        command("r r1");
        check(log.find(':') != std::string::npos, "register prompt");
        command("F");
        check(machine.cpu.reg(1) == 15, "hexadecimal F / register prompt");
        command("r rzr=1");
        check(log.starts_with("Error:") && machine.cpu.reg(15) == 0, "RZR protection");
        command("r pc=3");
        check(log.starts_with("Error:") && machine.cpu.state.pc == 0x100, "PC alignment");
        command("eb 0:4000 12 34 56 78");
        check(bus.peek32(0x4000) == 0x78563412, "little endian byte edit");
        command("ew 4001 AABB");
        check(bus.peek32(0x4000) == 0x78aabb12, "unaligned word edit");
        command("ed 4004 DEADBEEF");
        command("dd 0:4000 L2");
        check(log.find("78AABB12 DEADBEEF") != std::string::npos, "DD Lcount");
        command("d");
        check(log.starts_with("0000:4008"), "D continues previous format/address");
        command("db 0:4000 4003");
        check(log.find("12 BB AA 78") != std::string::npos, "inclusive range");
        command("e 4000");
        command("");
        check(bus.peek32(0x4000) == 0x78aabb12, "empty edit cancels");
        command("eb 4000 FF 100");
        check(log.starts_with("Error:") && bus.peek32(0x4000) == 0x78aabb12,
              "invalid edit is atomic");
        command("ed FE:010C 1");
        check(log.starts_with("Error:"), "MMIO edit rejected");
        command("db FE:0004 L1");
        check(log.find("41") != std::string::npos && console.read(4) == 65,
              "memory dump does not consume console input");
        const auto phase = fdc.phase();
        command("dd FE:0104 L1");
        check(fdc.phase() == phase, "FDC dump does not change phase");
        const auto time = machine.clock.now();
        const auto snapshot = debugger.snapshot();
        check(snapshot.find("FDC:") != std::string::npos && machine.clock.now() == time,
              "snapshot is non-consuming");
        command("ed 0:100 442F0005 48210800 18000000");
        command("u 0:100 L3");
        check(log.find("ADI R1,RZR,0005H") != std::string::npos &&
                  log.find("SUB R1,R1,R1") != std::string::npos &&
                  log.find("JMP 0108H") != std::string::npos,
              "native disassembly");
        command("t=0:100 2");
        execute();
        check(machine.cpu.reg(1) == 0 && steps == 2 && machine.cpu.state.pc == 0x108,
              "T address and instruction count");
        command("bp3 0:108 1");
        command("bl");
        check(log.find("3 e 0000:0108 0001") != std::string::npos, "sticky breakpoint syntax");
        command("g=0:100");
        execute();
        check(steps == 5 && machine.cpu.state.pc == 0x108, "breakpoint pass count");
        command("bd 3");
        command("bl");
        check(log.find("3 d") != std::string::npos, "disable breakpoint");
        command("be 3");
        command("bc *");
        command("g=0:100 0:108");
        execute();
        check(machine.cpu.state.pc == 0x108, "temporary Go breakpoint");
        command("p=0:100");
        execute();
        check(machine.cpu.reg(1) == 5 && machine.cpu.state.pc == 0x104, "P native instruction");
        // A real Call expansion and its writable return slot, not a mocked step.
        command("r r12=8000");
        command("r sp=F000");
        command("ed 0:200 044C820C 0C400000 18000039 181FFFE2 00000000");
        command("ed 0:280 20400000 084C8288 18000000");
        command("ed 0:2EC 442F0009 181FFFE4");
        command("p=0:200");
        execute();
        check(machine.cpu.state.pc == 0x210 && machine.cpu.reg(1) == 9 &&
                  machine.cpu.reg(14) == 0xf000,
              "P steps over canonical Call including native return");
        command("bp0 0:2EC");
        command("p=0:200");
        execute();
        check(machine.cpu.state.pc == 0x2ec && machine.cpu.reg(14) == 0xeffc,
              "P honors sticky breakpoints inside subroutine");
        command("bc 0");
        command("g 0:210");
        execute();
        check(machine.cpu.reg(14) == 0xf000, "continue from breakpoint within Call");
        command("pause");
        check(log.find("Unsupported SYMDEB command") != std::string::npos,
              "non-SYMDEB commands rejected");
        command("help");
        check(log.starts_with("Error:"), "invented help command rejected");
        command("?");
        check(log.find("BP[n]") != std::string::npos, "SYMDEB help");
        command("? 10");
        check(log.find("(16)") != std::string::npos, "SYMDEB hexadecimal display");
        command("q");
        check(debugger.quitting(), "Q exits");
        std::cout << "PASS monitor: SYMDEB parsing, registers, memory, non-consuming MMIO, "
                     "disassembly, trace/proceed and breakpoints\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}

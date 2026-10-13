#pragma once
#include "devices/fdc/fdc.hpp"
#include "time/machine.hpp"
#include <array>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace yosic {
// All access happens on the emulation thread, at instruction boundaries.
class Debugger {
  public:
    Debugger(Machine &machine, Bus &bus, Fdc &fdc, std::uint64_t &steps);
    bool running() const { return running_; }
    bool accepts_guest_input() const { return running_ && !tracing_ && !proceeding_; }
    bool quitting() const { return quitting_; }
    bool awaiting_value() const {
        return pending_register_.has_value() || pending_edit_.has_value();
    }
    void interrupt();
    void command(std::string_view line);
    bool advance();
    std::string snapshot() const;
    std::function<void(const std::string &)> output;

  private:
    struct Breakpoint {
        std::uint32_t address;
        std::uint32_t passes = 0;
        bool enabled = true;
    };
    Machine &machine_;
    Bus &bus_;
    Fdc &fdc_;
    std::uint64_t &steps_;
    bool running_ = true, quitting_ = false, skip_break_ = false;
    std::uint32_t tracing_ = 0;
    std::uint32_t proceeding_ = 0;
    std::optional<std::uint32_t> proceed_return_;
    std::array<std::optional<Breakpoint>, 10> breaks_;
    std::vector<std::uint32_t> temporary_;
    std::optional<unsigned> pending_register_;
    std::optional<std::uint32_t> pending_edit_;
    unsigned dump_width_ = 1, edit_width_ = 1;
    std::uint32_t dump_next_ = 0x4000, unassemble_next_ = 0, watch_ = 0x4000;
    std::uint16_t dump_segment_ = 0, unassemble_segment_ = 0;
    bool dumped_ = false, unassembled_ = false;
    void emit(const std::string &text) const;
    std::uint32_t value(std::string_view text) const;
    std::uint32_t address(std::string_view text, std::uint16_t segment) const;
    unsigned register_index(std::string_view text) const;
    std::uint32_t register_value(unsigned index) const;
    void set_register(unsigned index, std::uint32_t value);
    std::uint32_t read(std::uint32_t at, unsigned width) const;
    void edit(std::uint32_t at, const std::vector<std::uint32_t> &data, unsigned width);
    std::string registers() const;
    std::string disassemble(std::uint32_t at) const;
};
} // namespace yosic

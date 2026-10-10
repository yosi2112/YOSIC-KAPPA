#pragma once
#include "devices/fdc/fdc.hpp"
#include <ostream>

namespace yosic {
// Observes the same board accesses as the guest; never consumes extra FDC bytes.
class FdcPort final : public Device {
    Fdc &fdc_;
    Clock &clock_;
    FdcBoard board_;
    std::ostream *trace_;
    void log(char operation, std::uint32_t offset, std::uint32_t value, Fdc::Phase phase) {
        if (!trace_ || (offset != 4 && offset != 12 && offset != 16))
            return;
        const auto flags = trace_->flags();
        *trace_ << "fdc time_ns=" << std::dec << clock_.now() << " " << operation << " offset=0x"
                << std::hex << offset << " value=0x" << value << std::dec
                << " phase=" << static_cast<unsigned>(phase) << " msr=0x" << std::hex
                << unsigned(fdc_.msr()) << " signals=0x" << fdc_.signal_bits() << '\n';
        trace_->flags(flags);
    }

  public:
    FdcPort(Fdc &fdc, Clock &clock, std::ostream *trace)
        : fdc_(fdc), clock_(clock), board_(fdc), trace_(trace) {}
    std::uint32_t read(std::uint32_t offset) override {
        const auto phase = fdc_.phase();
        const auto value = board_.read(offset);
        log('R', offset, value, phase);
        return value;
    }
    void write(std::uint32_t offset, std::uint32_t value) override {
        const auto phase = fdc_.phase();
        board_.write(offset, value);
        log('W', offset, value, phase);
    }
    std::uint32_t peek(std::uint32_t offset) const override { return board_.peek(offset); }
    Signals signals() const override { return board_.signals(); }
};
} // namespace yosic

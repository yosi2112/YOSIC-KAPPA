#pragma once
#include "bus/mmio.hpp"
#include <deque>
#include <functional>
#include <utility>
namespace yosic {
class Console : public Device {
    std::deque<std::uint8_t> input_;
    bool eof_ = false;
    std::function<void(std::uint8_t)> output_;

  public:
    static constexpr std::uint32_t base = 0x00fe0000;
    explicit Console(std::function<void(std::uint8_t)> output) : output_(std::move(output)) {
        if (!output_)
            throw std::invalid_argument("console output sink required");
    }
    void feed(std::span<const std::uint8_t> bytes) {
        if (eof_)
            throw std::invalid_argument("console input already closed");
        input_.insert(input_.end(), bytes.begin(), bytes.end());
    }
    void close_input() { eof_ = true; }
    bool input_closed() const { return eof_; }
    std::uint32_t peek(std::uint32_t offset) const override {
        if (offset == 0)
            return (input_.empty() ? 0u : 1u) | 2u | (eof_ ? 4u : 0u);
        if (offset == 4)
            return input_.empty() ? 0u : input_.front();
        throw Fault("invalid-console-read", base + offset);
    }
    std::uint32_t read(std::uint32_t offset) override {
        const auto value = peek(offset);
        if (offset == 4 && !input_.empty())
            input_.pop_front();
        return value;
    }
    void write(std::uint32_t offset, std::uint32_t value) override {
        if (offset != 8)
            throw Fault("invalid-console-write", base + offset);
        output_(static_cast<std::uint8_t>(value));
    }
};
} // namespace yosic

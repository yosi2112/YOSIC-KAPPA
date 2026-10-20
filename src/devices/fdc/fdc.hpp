#pragma once
#include "bus/mmio.hpp"
#include "drives/drive.hpp"
#include "time/clock.hpp"
#include <deque>
#include <functional>
namespace yosic {
class Fdc {
  public:
    enum class Phase { idle, command, execution, result };
    explicit Fdc(Clock &clock);
    Fdc(const Fdc &) = delete;
    Fdc &operator=(const Fdc &) = delete;
    std::array<Drive, 4> drives;
    std::uint8_t msr() const;
    std::uint8_t data(bool dack = false);
    void data(std::uint8_t value, bool dack = false);
    std::uint8_t peek_data() const;
    Signals signals() const;
    void control(std::uint32_t value);
    std::uint32_t signal_bits() const;
    Phase phase() const { return phase_; }
    bool can_change_media(unsigned unit) const;
    void change_media(unsigned unit, std::shared_ptr<media::Disk> disk, bool read_only,
                      bool double_sided);

  private:
    Clock &clock_;
    std::shared_ptr<int> life_ = std::make_shared<int>(0);
    Phase phase_ = Phase::idle;
    std::vector<std::uint8_t> command_;
    std::deque<std::uint8_t> result_;
    struct Interrupt {
        std::uint8_t status, cylinder;
    };
    std::deque<Interrupt> interrupts_;
    std::array<std::uint8_t, 4> pcn_{};
    std::array<bool, 4> busy_{}, ready_{}, loaded_{};
    std::array<std::uint64_t, 4> seek_epoch_{}, unload_epoch_{};
    std::uint64_t epoch_ = 0, byte_epoch_ = 0, format_start_ = 0;
    std::uint8_t specify1_ = 0, specify2_ = 0, op_ = 0, st1_ = 0, st2_ = 0;
    std::array<std::uint8_t, 4> id_{};
    unsigned unit_ = 0, head_ = 0, poll_unit_ = 0, track_count_ = 0, format_count_ = 0;
    std::size_t sector_ = 0, offset_ = 0, transfer_ = 0, sector_offset_ = 0;
    std::uint8_t latch_ = 0;
    bool reset_ = false, slow_ = false, request_ = false, drq_ = false, result_irq_ = false;
    bool reading_ = false, tc_ = false, scan_equal_ = true, scan_low_ = true, scan_high_ = true;
    bool marked_ = false, track_match_ = false, media_ready_ = false;
    std::uint64_t next_byte_ = 0;
    void result_id();
    std::vector<std::uint8_t> format_id_;
    void later(std::uint64_t delay, std::function<void()> action);
    void event(std::uint64_t delay, std::function<void()> action);
    void poll();
    void interrupt(std::uint8_t status, unsigned unit);
    void reset(bool held);
    void execute();
    void result(std::initializer_list<std::uint8_t> values, bool irq);
    void finish(std::uint8_t s1 = 0, std::uint8_t s2 = 0, std::uint8_t s0 = 0);
    void unload();
    void seek(unsigned unit, unsigned head, unsigned target, bool recal, unsigned steps = 0);
    bool ready();
    bool writing() const { return op_ == 5 || op_ == 9 || op_ == 13; }
    bool scan() const { return op_ == 17 || op_ == 25 || op_ == 29; }
    bool ndma() const { return (specify2_ & 1) != 0; }
    std::uint64_t byte_time() const;
    void start_media();
    void find_sector();
    void found_sector();
    void request_byte();
    void accept_byte(std::uint8_t value);
    void end_sector();
    void next_sector();
    void terminal_count();
    void format_begin();
    void format_record();
    void format_end();
    media::Sector &sector();
};
class FdcBoard final : public Device {
  public:
    static constexpr std::uint32_t base = 0x00fe0100;
    explicit FdcBoard(Fdc &fdc) : fdc_(fdc) {}
    std::uint32_t read(std::uint32_t offset) override;
    void write(std::uint32_t offset, std::uint32_t value) override;
    std::uint32_t peek(std::uint32_t offset) const override;
    Signals signals() const override { return fdc_.signals(); }

  private:
    Fdc &fdc_;
};
} // namespace yosic

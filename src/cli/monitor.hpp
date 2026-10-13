#pragma once
#include "debugger.hpp"

namespace yosic {
class HardwareMonitor {
  public:
    explicit HardwareMonitor(Debugger &debugger);
    ~HardwareMonitor();
    HardwareMonitor(const HardwareMonitor &) = delete;
    HardwareMonitor &operator=(const HardwareMonitor &) = delete;
    void pump();
    void interrupt();

  private:
    Debugger &debugger_;
    std::string line_;
    bool prompt_ = false, skip_lf_ = false;
};
} // namespace yosic

#include "monitor.hpp"
#include <conio.h>
#include <iostream>

namespace yosic {
HardwareMonitor::HardwareMonitor(Debugger &debugger) : debugger_(debugger) {
    debugger_.interrupt();
    debugger_.output = [](const std::string &text) {
        std::cout << text << std::flush;
        if (!std::cout)
            throw std::runtime_error("monitor output failed");
    };
    debugger_.output("YOSIC-KAPPA Hardware Monitor / Debugger\n"
                     "SYMDEB command syntax. ? lists commands; G starts execution.\n"
                     "Ctrl+Break returns to the monitor; Q exits.\n");
}
HardwareMonitor::~HardwareMonitor() {
    debugger_.output = {};
}
void HardwareMonitor::interrupt() {
    line_.clear();
    prompt_ = false;
    debugger_.interrupt();
}
void HardwareMonitor::pump() {
    if (debugger_.running()) {
        prompt_ = false;
        return;
    }
    if (!prompt_) {
        debugger_.output(debugger_.snapshot());
        debugger_.output("-");
        prompt_ = true;
    }
    while (_kbhit()) {
        const auto ch = _getch();
        if (ch == 3 || ch == 26) {
            debugger_.command("Q");
            return;
        }
        if (ch == 0 || ch == 224) {
            (void)_getch();
            continue;
        }
        if (ch == 10 && skip_lf_) {
            skip_lf_ = false;
            continue;
        }
        skip_lf_ = false;
        if (ch == 13 || ch == 10) {
            debugger_.output("\n");
            const auto command = line_;
            line_.clear();
            skip_lf_ = ch == 13;
            debugger_.command(command);
            if (debugger_.running() || debugger_.quitting()) {
                prompt_ = false;
                return;
            }
            if (!debugger_.awaiting_value())
                debugger_.output("-");
        } else if (ch == 8 && !line_.empty()) {
            line_.pop_back();
            debugger_.output("\b \b");
        } else if (ch >= 32 && ch <= 126 && line_.size() < 1024) {
            line_ += static_cast<char>(ch);
            debugger_.output(std::string(1, static_cast<char>(ch)));
        }
    }
}
} // namespace yosic

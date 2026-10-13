#include "cli/run.hpp"
#include <iostream>
#include <string_view>

namespace {
constexpr std::string_view help_text =
    "YOSIC-KAPPA CLI simulator\n"
    "\n"
    "Usage: yosic-sim [--help | --version]\n"
    "\n"
    "  --help     Show this help.\n"
    "  --version  Show the program version.\n"
    "\n"
    "Run: yosic-sim --rom FILE [--steps N | --step] [--break ADDRESS]\n"
    "     Runs without an instruction limit by default; Ctrl+C stops execution.\n"
    "     --steps N runs at most N instructions (0 runs none); --step runs one.\n"
    "     [--registers] [--dump ADDRESS WORDS] [--trace] [--trace-fdc] [--input FILE | "
    "--interactive]\n"
    "     [--monitor | --no-monitor] CLI monitor starts paused on a terminal by default.\n"
    "Disk: --disk UNIT FILE | --disk-new UNIT; --disk-copy UNIT NEWFILE\n"
    "      --disk-index UNIT INDEX; --geometry C H S N (required for raw/new)\n"
    "CPU, assembler, MMIO console and FDC are available. Assembly IPL boots stage2 from raw/D88.\n";

int usage_error(std::string_view message) {
    std::cerr << "error: " << message << " Use --help.\n";
    return 2;
}
} // namespace

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "--rom")
            return yosic::run_cli(argc, argv);
    }
    if (argc > 2) {
        return usage_error("Expected at most one argument.");
    }
    if (argc == 1 || std::string_view(argv[1]) == "--help") {
        std::cout << help_text;
        return 0;
    }
    if (std::string_view(argv[1]) == "--version") {
        std::cout << "yosic-sim " YOSIC_VERSION "\n";
        return 0;
    }
    return usage_error("Unknown argument.");
}

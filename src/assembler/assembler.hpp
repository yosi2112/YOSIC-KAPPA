#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
namespace yosic {
struct Assembly {
    std::vector<std::uint8_t> bytes;
    std::string listing;
    std::string map;
    std::vector<std::filesystem::path> dependencies;
};
struct AssemblyOptions {
    std::filesystem::path source_path;
    std::vector<std::filesystem::path> include_directories;
};
Assembly assemble(std::string_view source, std::uint32_t base = 0,
                  const AssemblyOptions &options = {});
} // namespace yosic

#include "assembler.hpp"
#include "outputs.hpp"
#include "source.hpp"
#include <charconv>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>

namespace {
std::wstring path_key(const std::filesystem::path &path) {
    auto text = path.wstring();
    for (auto &ch : text)
        ch = static_cast<wchar_t>(std::towlower(ch));
    return text;
}
} // namespace
int main(int argc, char *argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        std::cout << "Usage: yosic-as INPUT -o OUTPUT [--base ADDRESS] [--list FILE] [--map FILE] "
                     "[-I DIR ...]\n";
        return 0;
    }
    try {
        if (argc < 4)
            throw std::invalid_argument("expected input and -o output");
        const std::filesystem::path input_path(argv[1]);
        std::filesystem::path output_path, list_path, map_path;
        std::uint32_t base = 0;
        yosic::AssemblyOptions options;
        options.source_path = input_path;
        std::set<std::string> seen;
        for (int i = 2; i < argc; ++i) {
            const std::string option(argv[i]);
            if ((option != "-I" && !seen.insert(option).second) || ++i >= argc)
                throw std::invalid_argument("duplicate option or missing value");
            if (option == "-I")
                options.include_directories.emplace_back(argv[i]);
            else if (option == "-o")
                output_path = argv[i];
            else if (option == "--list")
                list_path = argv[i];
            else if (option == "--map")
                map_path = argv[i];
            else if (option == "--base") {
                std::string_view value(argv[i]);
                int radix = 10;
                if (value.starts_with("0x") || value.starts_with("0X")) {
                    value.remove_prefix(2);
                    radix = 16;
                }
                auto parsed =
                    std::from_chars(value.data(), value.data() + value.size(), base, radix);
                if (value.empty() || parsed.ec != std::errc{} ||
                    parsed.ptr != value.data() + value.size())
                    throw std::invalid_argument("invalid base address");
            } else
                throw std::invalid_argument("unknown option");
        }
        if (output_path.empty())
            throw std::invalid_argument("-o is required");
        std::vector<std::filesystem::path> paths{std::filesystem::weakly_canonical(input_path)};
        for (const auto &path : {output_path, list_path, map_path})
            if (!path.empty()) {
                const auto resolved = std::filesystem::weakly_canonical(path);
                for (const auto &other : paths) {
                    if (path_key(resolved) == path_key(other) ||
                        (std::filesystem::exists(resolved) && std::filesystem::exists(other) &&
                         std::filesystem::equivalent(resolved, other)))
                        throw std::invalid_argument("input/output paths must be distinct");
                }
                paths.push_back(resolved);
            }
        std::ifstream input(input_path, std::ios::binary | std::ios::ate);
        if (!input)
            throw std::runtime_error("cannot open source");
        if (input.tellg() > 16 * 1024 * 1024)
            throw std::invalid_argument("source exceeds 16 MiB");
        input.seekg(0);
        const std::string source{std::istreambuf_iterator<char>(input),
                                 std::istreambuf_iterator<char>()};
        if (input.bad())
            throw std::runtime_error("cannot read source");
        const auto assembly = yosic::assemble(source, base, options);
        for (const auto &dependency : assembly.dependencies)
            for (const auto &output : {output_path, list_path, map_path})
                if (!output.empty() && yosic::as_syntax::same_file(dependency, output))
                    throw std::invalid_argument(
                        "input/output paths must be distinct (include dependency)");
        yosic::AssemblyOutputs outputs;
        outputs.stage(output_path, reinterpret_cast<const char *>(assembly.bytes.data()),
                      assembly.bytes.size());
        if (!list_path.empty())
            outputs.stage(list_path, assembly.listing.data(), assembly.listing.size());
        if (!map_path.empty())
            outputs.stage(map_path, assembly.map.data(), assembly.map.size());
        outputs.commit();
        return 0;
    } catch (const std::invalid_argument &error) {
        std::cerr << "error: " << error.what() << '\n';
        return 2;
    } catch (const std::exception &error) {
        std::cerr << "error: " << error.what() << '\n';
        return 3;
    }
}

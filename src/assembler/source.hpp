#pragma once
#include "assembler.hpp"
#include "syntax.hpp"
#include <cwctype>
#include <fstream>
#include <optional>
#include <sstream>

namespace yosic::as_syntax {
inline bool same_file(const std::filesystem::path &a, const std::filesystem::path &b) {
    if (a.empty() || b.empty())
        return false;
    const auto key = [](const std::filesystem::path &p) {
        auto s = std::filesystem::weakly_canonical(p).wstring();
        for (auto &c : s)
            c = static_cast<wchar_t>(std::towlower(c));
        return s;
    };
    return key(a) == key(b) || (std::filesystem::exists(a) && std::filesystem::exists(b) &&
                                std::filesystem::equivalent(a, b));
}
struct SourceLine {
    std::string text;
    std::size_t number;
    std::filesystem::path path;
    std::string file;
    std::optional<std::string> route;
    bool boundary = false;
    std::size_t branch_floor = 0;
};
class SourceFiles {
    AssemblyOptions options_;
    std::filesystem::path directory_;
    std::map<std::filesystem::path, std::string> contents_;
    std::size_t total_ = 0;

  public:
    explicit SourceFiles(const AssemblyOptions &options) : options_(options) {
        directory_ = options.source_path.empty()
                         ? std::filesystem::current_path()
                         : std::filesystem::absolute(options.source_path).parent_path();
    }
    std::vector<std::filesystem::path> dependencies;
    std::string display(const std::filesystem::path &path) const {
        if (path.empty())
            return "<input>";
        auto relative = std::filesystem::absolute(path).lexically_relative(directory_);
        if (relative.empty() || relative.is_absolute())
            relative = path.filename();
        const auto utf8 = relative.generic_u8string();
        return {utf8.begin(), utf8.end()};
    }
    std::vector<SourceLine> lines(std::string_view source,
                                  const std::filesystem::path &path) const {
        std::istringstream input{std::string(source)};
        std::vector<SourceLine> result;
        std::string raw;
        while (std::getline(input, raw)) {
            if (!raw.empty() && raw.back() == '\r')
                raw.pop_back();
            result.push_back({raw, result.size() + 1, path, display(path), {}, false, 0});
        }
        return result;
    }
    void root(std::string_view source) {
        if (source.size() > 16 * 1024 * 1024)
            error("source exceeds 16 MiB");
        total_ = source.size();
        if (!options_.source_path.empty()) {
            const auto path = std::filesystem::weakly_canonical(options_.source_path);
            contents_[path] = std::string(source);
            dependencies.push_back(path);
        }
    }
    std::filesystem::path resolve(const Line &line, bool text) const {
        auto filename = line.args.at(0);
        if (filename.size() >= 2 && (filename.front() == '"' || filename.front() == '\'')) {
            if (filename.back() != filename.front())
                error("invalid include filename");
            filename = filename.substr(1, filename.size() - 2);
        }
        if (filename.empty() || filename.find('\0') != std::string::npos ||
            filename.find_first_of("\r\n\"'") != std::string::npos)
            error("invalid include filename");
        auto path = std::filesystem::path(std::u8string(filename.begin(), filename.end()));
        if (text && !path.has_extension())
            path += ".inc";
        const auto parent = line.path.empty() ? directory_ : line.path.parent_path();
        std::vector<std::filesystem::path> candidates{parent / path};
        if (!path.has_parent_path())
            for (const auto &directory : options_.include_directories)
                candidates.push_back(directory / path);
        for (const auto &candidate : candidates)
            if (std::filesystem::is_regular_file(candidate))
                return std::filesystem::canonical(candidate);
        throw std::runtime_error("cannot open include: " + filename);
    }
    const std::string &read(const std::filesystem::path &path) {
        if (const auto it = contents_.find(path); it != contents_.end())
            return it->second;
        std::ifstream input(path, std::ios::binary | std::ios::ate);
        if (!input)
            throw std::runtime_error("cannot open include: " + display(path));
        const auto end = input.tellg();
        if (end < 0)
            throw std::runtime_error("cannot size include: " + display(path));
        if (static_cast<std::uint64_t>(end) > 16 * 1024 * 1024 - total_)
            error("include input exceeds 16 MiB");
        std::string data(static_cast<std::size_t>(end), '\0');
        input.seekg(0);
        if (!data.empty() && !input.read(data.data(), static_cast<std::streamsize>(data.size())))
            throw std::runtime_error("cannot read include: " + display(path));
        total_ += data.size();
        dependencies.push_back(path);
        return contents_.emplace(path, std::move(data)).first->second;
    }
};
} // namespace yosic::as_syntax

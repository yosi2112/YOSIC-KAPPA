#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace yosic {
// Prepare every output before replacing any of the caller's existing files.
class AssemblyOutputs {
    struct Entry {
        std::filesystem::path target, directory;
        bool backed_up = false, installed = false;
    };
    std::vector<Entry> entries_;

  public:
    AssemblyOutputs() = default;
    AssemblyOutputs(const AssemblyOutputs &) = delete;
    AssemblyOutputs &operator=(const AssemblyOutputs &) = delete;
    ~AssemblyOutputs() {
        for (const auto &entry : entries_) {
            std::error_code ignored;
            std::filesystem::remove(entry.directory / "new", ignored);
            // Retain a backup if rollback itself encountered a filesystem failure.
            if (!entry.backed_up)
                std::filesystem::remove(entry.directory / "old", ignored);
            std::filesystem::remove(entry.directory, ignored);
        }
    }
    void stage(const std::filesystem::path &path, const char *data, std::size_t size) {
        if (path.empty())
            return;
        const auto target = std::filesystem::weakly_canonical(path);
        if (std::filesystem::exists(target) && !std::filesystem::is_regular_file(target))
            throw std::runtime_error("output is not a regular file: " + path.string());
        const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
        std::filesystem::path directory;
        for (unsigned attempt = 0; attempt < 32; ++attempt) {
            auto candidate = target.parent_path() /
                             (".yosic-as-" + std::to_string(nonce) + "-" + std::to_string(attempt));
            if (std::filesystem::create_directory(candidate)) {
                directory = std::move(candidate);
                break;
            }
        }
        if (directory.empty())
            throw std::runtime_error("cannot create output staging directory");
        entries_.push_back({target, directory});
        std::ofstream output(directory / "new", std::ios::binary);
        if (!output || (size && !output.write(data, static_cast<std::streamsize>(size))))
            throw std::runtime_error("cannot write " + path.string());
        output.close();
        if (!output)
            throw std::runtime_error("cannot close " + path.string());
    }
    void commit() {
        try {
            for (auto &entry : entries_) {
                if (std::filesystem::exists(entry.target)) {
                    std::filesystem::rename(entry.target, entry.directory / "old");
                    entry.backed_up = true;
                }
                std::filesystem::rename(entry.directory / "new", entry.target);
                entry.installed = true;
            }
        } catch (const std::exception &failure) {
            std::string recovery;
            for (auto it = entries_.rbegin(); it != entries_.rend(); ++it) {
                std::error_code ec;
                if (it->installed)
                    std::filesystem::remove(it->target, ec);
                if (!ec && it->backed_up) {
                    std::filesystem::rename(it->directory / "old", it->target, ec);
                    if (!ec)
                        it->backed_up = false;
                }
                if (ec)
                    recovery += " retained backup: " + (it->directory / "old").string();
            }
            throw std::runtime_error(std::string(failure.what()) + recovery);
        }
        for (auto &entry : entries_)
            entry.backed_up = false;
    }
};
} // namespace yosic

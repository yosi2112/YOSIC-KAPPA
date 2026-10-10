#include "media/media.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {
using namespace yosic::media;
std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}
void put(Bytes &bytes, std::size_t at, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i)
        bytes.at(at + i) = static_cast<std::uint8_t>(value >> (8 * i));
}
Bytes make_image(const Bytes &payload, bool d88) {
    auto disk = std::make_unique<Disk>(create({}));
    auto &sectors = disk->tracks[0].sectors;
    std::uint32_t checksum = 0;
    for (std::size_t i = 0; i < payload.size(); i += 4)
        for (unsigned j = 0; j < 4; ++j)
            checksum += std::uint32_t(payload[i + j]) << (8 * j);
    const std::uint32_t header[]{0x3150424b,
                                 1,
                                 static_cast<std::uint32_t>(payload.size()),
                                 0x2000,
                                 0x2000,
                                 checksum,
                                 static_cast<std::uint32_t>((payload.size() + 511) / 512),
                                 0};
    for (std::size_t i = 0; i < 8; ++i)
        put(sectors[0].data, i * 4, header[i]);
    for (std::size_t i = 0; i < payload.size(); ++i)
        sectors[1 + i / 512].data[i % 512] = payload[i];
    if (!d88)
        return write_raw(*disk, {});
    D88 container;
    container.disks.push_back(std::move(*disk));
    return write_d88(container);
}
} // namespace
int main(int argc, char **argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--help") {
            std::cout << "Usage: yosic-mkimage STAGE2.bin -o BOOT.img|BOOT.d88 [--overwrite]\n"
                         "KPB1 boot image: 80x2x18x512 MFM; payload 4..8192 bytes, aligned to 4.\n";
            return 0;
        }
        if (argc < 4)
            throw std::invalid_argument("missing input/output; use --help");
        const std::filesystem::path input(argv[1]);
        std::filesystem::path output;
        bool overwrite = false;
        for (int i = 2; i < argc; ++i) {
            const std::string option(argv[i]);
            if (option == "-o" && output.empty() && i + 1 < argc)
                output = argv[++i];
            else if (option == "--overwrite" && !overwrite)
                overwrite = true;
            else
                throw std::invalid_argument("unknown, duplicate or incomplete option");
        }
        if (output.empty())
            throw std::invalid_argument("missing output");
        const auto extension = lower(output.extension().string());
        if (extension != ".img" && extension != ".d88")
            throw std::invalid_argument("output extension must be .img or .d88");
        const auto normalized = [](const std::filesystem::path &p) {
            return lower(std::filesystem::weakly_canonical(p).string());
        };
        if (normalized(input) == normalized(output) ||
            (std::filesystem::exists(output) && std::filesystem::equivalent(input, output)))
            throw std::invalid_argument("output aliases input");
        if (std::filesystem::exists(output) && !overwrite)
            throw std::invalid_argument("output exists; use --overwrite explicitly");
        const auto size = std::filesystem::file_size(input);
        if (size < 4 || size > 8192 || size % 4)
            throw std::invalid_argument("payload must be 4..8192 bytes and aligned to 4");
        const auto payload = load(input);
        if (payload.size() != size)
            throw std::runtime_error("input size changed while reading");
        const auto image = make_image(payload, extension == ".d88");
        // Complete validation and encoding before opening an explicitly replaceable output.
        if (!overwrite)
            save_new(output, image);
        else {
            std::ofstream stream(output, std::ios::binary | std::ios::trunc);
            if (!stream || !stream.write(reinterpret_cast<const char *>(image.data()),
                                         static_cast<std::streamsize>(image.size())))
                throw std::runtime_error("cannot write boot image");
            stream.close();
            if (!stream)
                throw std::runtime_error("cannot close boot image");
        }
        std::cout << "KPB1: " << payload.size() << " bytes -> " << output.string() << '\n';
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << '\n';
        return 2;
    }
}

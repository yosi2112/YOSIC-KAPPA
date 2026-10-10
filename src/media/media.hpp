#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>
namespace yosic::media {
using Bytes = std::vector<std::uint8_t>;
struct Geometry {
    unsigned cylinders = 80, heads = 2, sectors = 18, n = 2;
};
struct Sector {
    std::array<std::uint8_t, 4> id{};
    std::uint8_t density = 0, deleted = 0, status = 0;
    std::array<std::uint8_t, 5> reserved{};
    Bytes data;
    bool id_mark = true, data_mark = true, id_crc = true, data_crc = true;
    std::uint32_t position = 0, gap = 0; // ns after index; 0 position implies canonical placement
    bool operator==(const Sector &) const = default;
};
struct Track {
    std::vector<Sector> sectors;
    std::uint32_t revolution = 200000000;
    std::uint32_t rate = 0; // MFM bit/s, 0 derives from disk type (FM transfers at half rate)
    bool explicit_layout = false;
    bool operator==(const Track &) const = default;
};
struct Disk {
    std::array<std::uint8_t, 26> header{}; // D88 name and reserved bytes
    std::uint8_t protect = 0, type = 0x20;
    std::array<Track, 164> tracks;
    Bytes original;
    bool dirty = false;
};
struct D88 {
    std::vector<Disk> disks;
};
std::uint32_t canonical_position(const Track &, std::size_t, std::uint8_t type);
unsigned canonical_gap(const Sector &);
Disk create(Geometry geometry, std::uint8_t fill = 0);
Disk read_raw(std::span<const std::uint8_t>, Geometry);
Bytes write_raw(const Disk &, Geometry);
D88 read_d88(std::span<const std::uint8_t>);
Bytes write_d88(const D88 &);
Bytes write_internal(const Disk &);
Disk read_internal(std::span<const std::uint8_t>);
void decode_status(Sector &);
std::uint8_t encode_status(const Sector &);
Bytes load(const std::filesystem::path &);
void save_new(const std::filesystem::path &, std::span<const std::uint8_t>);
std::uint16_t crc16(std::span<const std::uint8_t>, std::uint16_t initial = 0xffff);
} // namespace yosic::media

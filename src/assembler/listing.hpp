#pragma once
#include "syntax.hpp"
#include <iomanip>
#include <sstream>

namespace yosic::as_syntax {
inline std::string hex_address(std::uint64_t address) {
    std::ostringstream out;
    out << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << address;
    return out.str();
}
inline std::string listing_source(const std::string &text) {
    std::string out;
    std::size_t column = 0;
    for (const unsigned char c : text) {
        if (c == '\t') {
            const auto count = 8 - column % 8;
            out.append(count, ' ');
            column += count;
        } else {
            out += static_cast<char>(c);
            if ((c & 0xc0) != 0x80)
                ++column;
        }
    }
    return out;
}
inline void listing_line(std::ostream &out, const Line &line,
                         const std::vector<std::uint8_t> &bytes) {
    if (line.hidden)
        return;
    for (std::size_t offset = 0;; offset += 8) {
        std::ostringstream object;
        object << std::uppercase << std::hex << std::setfill('0');
        for (auto i = offset; i < std::min(offset + 8, bytes.size()); ++i)
            object << (i == offset ? "" : " ") << std::setw(2) << unsigned(bytes[i]);
        out << std::right << std::setfill(' ') << std::setw(8)
            << (offset ? "" : std::to_string(line.number)) << ' ' << (offset ? '+' : line.mark)
            << ' ' << std::setw(9) << hex_address(line.address + offset) << ' ' << std::left
            << std::setw(23) << object.str() << ' ';
        if (!offset) {
            out << listing_source(line.original) << " ; @" << line.file << ':' << line.number;
            if (!line.trace.empty())
                out << " via " << line.trace;
            if (!line.note.empty())
                out << " [" << line.note << ']';
        }
        out << '\n';
        if (offset + 8 >= bytes.size())
            break;
    }
}
} // namespace yosic::as_syntax

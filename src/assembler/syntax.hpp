#pragma once
#include "expression.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
namespace yosic::as_syntax {
inline std::string trim(std::string_view s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
        s.remove_prefix(1);
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.remove_suffix(1);
    return std::string(s);
}
inline std::string upper(std::string s) {
    for (auto &c : s)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}
inline bool name(std::string_view s) {
    return !s.empty() && s.size() <= 255 && !std::isdigit(static_cast<unsigned char>(s.front())) &&
           std::all_of(s.begin(), s.end(),
                       [](unsigned char c) { return std::isalnum(c) || c == '_' || c == '.'; });
}
inline const std::map<std::string, unsigned> opcodes{
    {"NOP", 0},    {"LRM", 1},    {"SRM", 2},    {"PSH", 3},    {"SRZ", 4},
    {"SNZ", 5},    {"JMP", 6},    {"POP", 8},    {"ADD", 0x10}, {"ADI", 0x11},
    {"SUB", 0x12}, {"ANL", 0x14}, {"ORR", 0x15}, {"XOR", 0x16}, {"LSEG", 0x20}};
inline bool known_op(const std::string &op) {
    static const std::vector<std::string> directives = {
        "EQU",    "=",      "SET",  ":=",    "EVAL",  "ORG",      "ALIGN",   "DB",      "DW",
        "DD",     "DS.B",   "DS.W", "DS.L",  "CPU",   "END",      "RELAXED", "MACRO",   "ENDM",
        "IRP",    "IRPC",   "REPT", "WHILE", "EXITM", "SHIFT",    "MAXNEST", "IF",      "IFDEF",
        "IFNDEF", "ELSEIF", "ELSE", "ENDIF", "LABEL", "FUNCTION", "INCLUDE", "BINCLUDE"};
    static const std::vector<std::string> legacy = {".EQU",  ".ORG",  ".ALIGN", ".SPACE",
                                                    ".BYTE", ".WORD", ".ASCII"};
    return opcodes.contains(op) || std::find(legacy.begin(), legacy.end(), op) != legacy.end() ||
           op.starts_with('!') ||
           std::find(directives.begin(), directives.end(), op) != directives.end();
}
inline std::vector<std::string> arguments(std::string_view text) {
    std::vector<std::string> out;
    if (trim(text).empty())
        return out;
    std::size_t start = 0;
    std::string nesting;
    char quote = 0;
    bool escape = false;
    for (std::size_t i = 0; i < text.size(); ++i) {
        char c = text[i];
        if (escape) {
            escape = false;
            continue;
        }
        if (quote) {
            if (c == '\\')
                escape = true;
            else if (c == quote)
                quote = 0;
        } else if (c == '\'' || c == '"')
            quote = c;
        else if (c == '(' || c == '[' || c == '{')
            nesting += c;
        else if (c == ')' || c == ']' || c == '}') {
            if (nesting.empty() || nesting.back() != (c == ')' ? '(' : c == ']' ? '[' : '{'))
                error("unmatched bracket");
            nesting.pop_back();
        } else if (c == ',' && nesting.empty()) {
            out.push_back(trim(text.substr(start, i - start)));
            start = i + 1;
        }
    }
    if (quote || !nesting.empty())
        error("unterminated quote or bracket");
    out.push_back(trim(text.substr(start)));
    return out;
}
struct Line {
    std::size_t number = 0;
    std::string label, op;
    std::vector<std::string> args;
    std::uint64_t address = 0, size = 0;
    bool relaxed = false;
    std::string scope;
    std::shared_ptr<const std::map<std::string, std::int64_t>> variables;
    std::string original, file = "<input>", trace, note, symbol;
    std::filesystem::path path;
    char mark = ' ';
    bool listing_only = false, hidden = false;
    std::vector<std::uint8_t> binary;
};
inline Line parse(std::string_view raw, std::size_t number) {
    char quote = 0;
    bool escape = false;
    std::size_t end = raw.size();
    for (std::size_t i = 0; i < raw.size(); ++i) {
        const char c = raw[i];
        if (escape) {
            escape = false;
            continue;
        }
        if (quote) {
            if (c == '\\')
                escape = true;
            else if (c == quote)
                quote = 0;
        } else if (c == '\'' || c == '"')
            quote = c;
        else if (c == ';') {
            end = i;
            break;
        }
    }
    if (quote)
        error("unterminated string");
    auto text = trim(raw.substr(0, end));
    Line line;
    line.number = number;
    if (text.empty())
        return line;
    const auto token = [&text]() {
        const auto split = text.find_first_of(" \t");
        auto head = text.substr(0, split);
        text = split == std::string::npos ? "" : trim(std::string_view(text).substr(split));
        return head;
    };
    auto head = token();
    const auto colon = head.find(':');
    if (colon != std::string::npos && head != ":=") {
        line.label = upper(head.substr(0, colon));
        text = trim(head.substr(colon + 1) + (text.empty() ? "" : " " + text));
        head = token();
    } else {
        const auto second = upper(text.substr(0, text.find_first_of(" \t")));
        // Existing column-1 mnemonics remain a compatibility extension.
        if ((!std::isspace(static_cast<unsigned char>(raw.front())) && !known_op(upper(head))) ||
            second == "EQU" || second == "=" || second == "SET" || second == ":=" ||
            second == "EVAL" || second == "MACRO" || second == "FUNCTION" || second == "LABEL") {
            line.label = upper(head);
            head = token();
        }
    }
    if (!line.label.empty() && !name(line.label))
        error("invalid label");
    line.op = upper(head);
    line.args = arguments(text);
    return line;
}
inline std::string render(const Line &line) {
    std::string text = line.label.empty() ? "    " : line.label + ": ";
    text += line.op;
    for (std::size_t i = 0; i < line.args.size(); ++i)
        text += (i ? "," : " ") + line.args[i];
    return text;
}
inline void arity(const Line &line, std::size_t count) {
    if (line.args.size() != count ||
        std::any_of(line.args.begin(), line.args.end(), [](const auto &s) { return s.empty(); }))
        throw std::invalid_argument("wrong operand count for " + line.op);
}
inline std::vector<std::uint8_t> literal(const std::string &s) {
    return string_bytes(s);
}
inline std::string integer_text(std::int64_t value) {
    return value == std::numeric_limits<std::int64_t>::min() ? "(-9223372036854775807-1)"
                                                             : "(" + std::to_string(value) + ")";
}
inline bool quoted_string(const std::string &s) {
    if (s.size() < 2 || (s[0] != '\'' && s[0] != '"'))
        return false;
    bool escape = false;
    for (std::size_t i = 1; i < s.size(); ++i) {
        if (escape)
            escape = false;
        else if (s[i] == '\\')
            escape = true;
        else if (s[i] == s[0])
            return i + 1 == s.size();
    }
    return false;
}
inline std::string replace_symbols(const std::string &text,
                                   const std::function<std::string(const std::string &)> &replace) {
    std::string out;
    char quote = 0;
    bool escape = false;
    for (std::size_t i = 0; i < text.size();) {
        char c = text[i];
        if (quote) {
            out += c;
            ++i;
            if (escape)
                escape = false;
            else if (c == '\\')
                escape = true;
            else if (c == quote)
                quote = 0;
        } else if (c == '\'' || c == '"') {
            quote = c;
            out += c;
            ++i;
        } else if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.') {
            const auto start = i++;
            while (i < text.size() && (std::isalnum(static_cast<unsigned char>(text[i])) ||
                                       text[i] == '_' || text[i] == '.'))
                ++i;
            auto token = text.substr(start, i - start);
            out += std::isdigit(static_cast<unsigned char>(token.front())) ? token : replace(token);
        } else {
            out += c;
            ++i;
        }
    }
    return out;
}
} // namespace yosic::as_syntax

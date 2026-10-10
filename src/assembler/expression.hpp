#pragma once
#include <bit>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace yosic::as_syntax {
[[noreturn]] inline void error(const char *message) {
    throw std::invalid_argument(message);
}
inline std::vector<std::uint8_t> string_bytes(std::string_view text) {
    if (text.size() < 2 || (text.front() != '\'' && text.front() != '"') ||
        text.back() != text.front())
        error("expected quoted string");
    std::vector<std::uint8_t> bytes;
    for (std::size_t i = 1; i + 1 < text.size(); ++i) {
        unsigned value = static_cast<unsigned char>(text[i]);
        if (text[i] == text.front())
            error("unescaped quote");
        if (text[i] == '\\') {
            if (++i + 1 >= text.size())
                error("incomplete string escape");
            const auto ch = static_cast<char>(std::tolower(static_cast<unsigned char>(text[i])));
            if (std::isdigit(static_cast<unsigned char>(ch)) || ch == 'x') {
                int base = ch == 'x' ? 16 : ch == '0' ? 8 : 10;
                unsigned limit = ch == 'x' ? 2 : 3;
                if (ch == 'x')
                    ++i;
                value = 0;
                unsigned count = 0;
                while (i + 1 < text.size() && count < limit) {
                    const auto digit_ch = static_cast<unsigned char>(
                        std::toupper(static_cast<unsigned char>(text[i])));
                    const unsigned digit = digit_ch >= '0' && digit_ch <= '9' ? digit_ch - '0'
                                           : digit_ch >= 'A' && digit_ch <= 'F'
                                               ? digit_ch - 'A' + 10
                                               : 99;
                    if (digit >= unsigned(base))
                        break;
                    value = value * unsigned(base) + digit;
                    ++i;
                    ++count;
                }
                if (!count || value > 255)
                    error("invalid numeric string escape");
                --i;
            } else {
                switch (ch) {
                case 'a':
                    value = 7;
                    break;
                case 'b':
                    value = 8;
                    break;
                case 'e':
                    value = 27;
                    break;
                case 't':
                    value = 9;
                    break;
                case 'n':
                    value = 10;
                    break;
                case 'r':
                    value = 13;
                    break;
                case '\\':
                    value = '\\';
                    break;
                case '\'':
                case 'h':
                    value = '\'';
                    break;
                case '"':
                case 'i':
                    value = '"';
                    break;
                default:
                    error("unsupported string escape");
                }
            }
        }
        bytes.push_back(static_cast<std::uint8_t>(value));
    }
    return bytes;
}
inline std::int64_t add(std::int64_t a, std::int64_t b) {
    constexpr auto lo = std::numeric_limits<std::int64_t>::min();
    constexpr auto hi = std::numeric_limits<std::int64_t>::max();
    if ((b > 0 && a > hi - b) || (b < 0 && a < lo - b))
        error("integer overflow");
    return a + b;
}
inline std::int64_t negate(std::int64_t a) {
    if (a == std::numeric_limits<std::int64_t>::min())
        error("integer overflow");
    return -a;
}
inline std::int64_t multiply(std::int64_t a, std::int64_t b) {
    constexpr auto lo = std::numeric_limits<std::int64_t>::min();
    constexpr auto hi = std::numeric_limits<std::int64_t>::max();
    if (a > 0 ? (b > 0 ? a > hi / b : b < lo / a)
              : (a < 0 && (b > 0 ? a < lo / b : b < 0 && a < hi / b)))
        error("integer overflow");
    return a * b;
}
class Expression {
    std::string_view text_;
    std::size_t at_ = 0;
    std::function<std::int64_t(std::string_view)> symbol_;
    std::function<std::int64_t(std::string_view, const std::vector<std::int64_t> &)> function_;
    bool relaxed_;
    bool evaluating_ = true;
    void space() {
        while (at_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[at_])))
            ++at_;
    }
    struct Op {
        std::string_view text;
        int binding;
    };
    Op op() const {
        // AS rank 1 is strongest; binding is reversed for precedence climbing.
        static constexpr Op ops[] = {{"<<", 12}, {">>", 12}, {"><", 11}, {"&&", 4}, {"||", 3},
                                     {"!!", 2},  {"<=", 1},  {">=", 1},  {"<>", 1}, {"==", 1},
                                     {"!=", 1},  {"&", 10},  {"|", 9},   {"!", 8},  {"^", 7},
                                     {"*", 6},   {"/", 6},   {"#", 6},   {"+", 5},  {"-", 5},
                                     {"<", 1},   {">", 1},   {"=", 1}};
        for (auto item : ops)
            if (text_.substr(at_).starts_with(item.text))
                return item;
        return {{}, 0};
    }
    std::int64_t atom(unsigned depth) {
        if (depth > 128)
            error("expression nesting too deep");
        space();
        if (at_ == text_.size())
            error("missing expression");
        const char ch = text_[at_];
        if (ch == '+' || ch == '-') {
            ++at_;
            const auto v = expression(6, depth + 1);
            return evaluating_ && ch == '-' ? negate(v) : v;
        }
        if (ch == '~') {
            ++at_;
            const bool logical = at_ < text_.size() && text_[at_] == '~';
            if (logical)
                ++at_;
            const auto v = expression(logical ? 13 : 14, depth + 1);
            return logical ? !v : ~v;
        }
        if (ch == '(') {
            ++at_;
            const auto v = expression(1, depth + 1);
            space();
            if (at_ == text_.size() || text_[at_++] != ')')
                error("missing closing parenthesis");
            return v;
        }
        if (ch == '\'' || ch == '"') {
            const auto start = at_++;
            bool escape = false;
            while (at_ < text_.size()) {
                char c = text_[at_++];
                if (escape)
                    escape = false;
                else if (c == '\\')
                    escape = true;
                else if (c == ch) {
                    const auto bytes = string_bytes(text_.substr(start, at_ - start));
                    if (bytes.empty() || bytes.size() > 4)
                        error("integer character constant requires 1..4 bytes");
                    std::int64_t value = 0;
                    for (auto b : bytes)
                        value = value * 256 + b;
                    return value;
                }
            }
            error("unterminated character constant");
        }
        if (ch == '$' && (!relaxed_ || at_ + 1 == text_.size() ||
                          !std::isxdigit(static_cast<unsigned char>(text_[at_ + 1])))) {
            ++at_;
            return evaluating_ ? symbol_("$") : 0;
        }
        int base = 10;
        bool numeric = std::isdigit(static_cast<unsigned char>(ch)) != 0;
        bool quoted = false, prefixed = false;
        if (relaxed_ && (ch == '$' || ch == '%' || ch == '@')) {
            base = ch == '$' ? 16 : ch == '%' ? 2 : 8;
            numeric = prefixed = true;
            ++at_;
        } else if (relaxed_ && at_ + 1 < text_.size() && text_[at_ + 1] == '\'') {
            const auto lower = std::tolower(static_cast<unsigned char>(ch));
            if (lower == 'h' || lower == 'x' || lower == 'b' || lower == 'o') {
                base = lower == 'b' ? 2 : lower == 'o' ? 8 : 16;
                numeric = quoted = prefixed = true;
                at_ += 2;
            }
        }
        if (numeric) {
            if (!prefixed && (text_.substr(at_, 2) == "0x" || text_.substr(at_, 2) == "0X" ||
                              text_.substr(at_, 2) == "0b" || text_.substr(at_, 2) == "0B")) {
                base = std::tolower(static_cast<unsigned char>(text_[at_ + 1])) == 'x' ? 16 : 2;
                prefixed = true;
                at_ += 2;
            }
            const auto start = at_;
            while (at_ < text_.size() && std::isalnum(static_cast<unsigned char>(text_[at_])))
                ++at_;
            auto end = at_;
            if (!prefixed && end > start) {
                const char suffix =
                    static_cast<char>(std::toupper(static_cast<unsigned char>(text_[end - 1])));
                if (suffix == 'H' || suffix == 'B' || suffix == 'O' || suffix == 'Q') {
                    base = suffix == 'H' ? 16 : suffix == 'B' ? 2 : 8;
                    --end;
                } else if (relaxed_ && end - start > 1 && text_[start] == '0')
                    base = 8;
            }
            if (quoted && (at_ == text_.size() || text_[at_++] != '\''))
                error("unterminated radix constant");
            std::int64_t value = 0;
            auto result = std::from_chars(text_.data() + start, text_.data() + end, value, base);
            if (start == end || result.ec != std::errc{} || result.ptr != text_.data() + end)
                error("invalid integer");
            return value;
        }
        const auto start = at_;
        while (at_ < text_.size() && (std::isalnum(static_cast<unsigned char>(text_[at_])) ||
                                      text_[at_] == '_' || text_[at_] == '.'))
            ++at_;
        if (start == at_)
            error("invalid expression token");
        const auto key = text_.substr(start, at_ - start);
        space();
        if (at_ < text_.size() && text_[at_] == '(') {
            ++at_;
            std::vector<std::int64_t> args;
            space();
            if (at_ < text_.size() && text_[at_] != ')') {
                for (;;) {
                    args.push_back(expression(1, depth + 1));
                    space();
                    if (at_ == text_.size() || text_[at_] != ',')
                        break;
                    ++at_;
                }
            }
            if (at_ == text_.size() || text_[at_++] != ')')
                error("missing function parenthesis");
            if (!evaluating_)
                return 0;
            if (!function_)
                error("unknown function");
            return function_(key, args);
        }
        return evaluating_ ? symbol_(key) : 0;
    }
    static std::int64_t apply(std::string_view op, std::int64_t a, std::int64_t b) {
        if (op == "+")
            return add(a, b);
        if (op == "-") {
            constexpr auto lo = std::numeric_limits<std::int64_t>::min();
            constexpr auto hi = std::numeric_limits<std::int64_t>::max();
            if ((b > 0 && a < lo + b) || (b < 0 && a > hi + b))
                error("integer overflow");
            return a - b;
        }
        if (op == "*")
            return multiply(a, b);
        if (op == "/" || op == "#") {
            if (!b)
                error("division by zero");
            if (a == std::numeric_limits<std::int64_t>::min() && b == -1)
                error("integer overflow");
            return op == "/" ? a / b : a % b;
        }
        if (op == "^") {
            if (b < 0)
                return 0;
            std::int64_t result = 1;
            while (b) {
                if (b & 1)
                    result = multiply(result, a);
                b >>= 1;
                if (b)
                    a = multiply(a, a);
            }
            return result;
        }
        if (op == "&")
            return a & b;
        if (op == "|")
            return a | b;
        if (op == "!")
            return a ^ b;
        if (op == "<<" || op == ">>") {
            if (b < 0 || b > 63)
                error("shift count must be 0..63");
            const auto bits = static_cast<std::uint64_t>(a);
            return op == "<<" ? std::bit_cast<std::int64_t>(bits << b) : a >> b;
        }
        if (op == "><") {
            if (b < 1 || b > 32)
                error("mirror width must be 1..32");
            const auto bits = static_cast<std::uint64_t>(a);
            std::uint64_t reversed = 0;
            for (std::int64_t i = 0; i < b; ++i)
                reversed = (reversed << 1) | ((bits >> i) & 1);
            const auto mask = (std::uint64_t(1) << b) - 1;
            return std::bit_cast<std::int64_t>((bits & ~mask) | reversed);
        }
        if (op == "&&")
            return a != 0 && b != 0;
        if (op == "||")
            return a != 0 || b != 0;
        if (op == "!!")
            return (a != 0) != (b != 0);
        if (op == "=" || op == "==")
            return a == b;
        if (op == "<>" || op == "!=")
            return a != b;
        if (op == "<")
            return a < b;
        if (op == ">")
            return a > b;
        if (op == "<=")
            return a <= b;
        if (op == ">=")
            return a >= b;
        error("unknown operator");
    }
    std::int64_t expression(int minimum, unsigned depth) {
        auto value = atom(depth);
        for (;;) {
            space();
            const auto item = op();
            if (item.binding < minimum)
                return value;
            at_ += item.text.size();
            const bool previous = evaluating_;
            const bool shorted =
                (item.text == "&&" && value == 0) || (item.text == "||" && value != 0);
            if (shorted)
                evaluating_ = false;
            const auto right = expression(item.binding + 1, depth + 1);
            evaluating_ = previous;
            value = previous ? apply(item.text, value, right) : 0;
        }
    }

  public:
    Expression(std::string_view text, std::function<std::int64_t(std::string_view)> symbol,
               bool relaxed = false,
               std::function<std::int64_t(std::string_view, const std::vector<std::int64_t> &)>
                   function = {})
        : text_(text), symbol_(std::move(symbol)), function_(std::move(function)),
          relaxed_(relaxed) {}
    std::int64_t value() {
        const auto result = expression(1, 0);
        space();
        if (at_ != text_.size())
            error("unexpected expression suffix");
        return result;
    }
};
} // namespace yosic::as_syntax

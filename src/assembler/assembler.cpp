#include "assembler.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <functional>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "listing.hpp"
#include "macros.hpp"
#include "syntax.hpp"
namespace yosic {
namespace {
using namespace as_syntax;
[[noreturn]] void fail(const std::string &text) {
    throw std::invalid_argument(text);
}
unsigned reg(std::string text) {
    text = upper(trim(text));
    if (text == "SP")
        return 14;
    if (text == "RZR")
        return 15;
    if (text.size() < 2 || text[0] != 'R')
        fail("expected register");
    unsigned result = 0;
    const auto parsed = std::from_chars(text.data() + 1, text.data() + text.size(), result);
    if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size() || result >= 16)
        fail("invalid register");
    return result;
}
} // namespace
Assembly assemble(std::string_view source, std::uint32_t base, const AssemblyOptions &options) {
    if ((base & 0xffffu) != 0)
        fail("base must be aligned to 64 KiB");
    SourceFiles files(options);
    std::vector<Line> lines;
    struct Constant {
        std::string expression, scope;
        std::uint64_t pc;
        bool relaxed;
        std::shared_ptr<const std::map<std::string, std::int64_t>> variables;
    };
    std::map<std::string, Constant> constants;
    struct Function {
        std::vector<std::string> parameters;
        Constant body;
    };
    std::map<std::string, Function> functions;
    std::map<std::string, std::int64_t> labels, variables;
    using Variables = std::shared_ptr<const std::map<std::string, std::int64_t>>;
    Variables variable_snapshot = std::make_shared<const std::map<std::string, std::int64_t>>();
    std::set<std::string> names{"TRUE", "FALSE"};
    std::size_t active_line = 1;
    std::string active_file, active_trace;
    std::uint64_t pc = base;
    bool relaxed = false;
    std::string scope;
    const auto qualify = [](std::string key, const std::string &local) {
        key = upper(key);
        return key.starts_with('.') ? local + key : key;
    };
    try {
        const auto evaluate = [&](const std::string &expression, std::uint64_t address,
                                  const std::string &local, bool mode, Variables snapshot = {}) {
            std::set<std::string> visiting;
            unsigned function_depth = 0;
            std::function<std::int64_t(const std::string &, std::uint64_t, const std::string &,
                                       bool, Variables)>
                eval;
            eval = [&](const std::string &expr, std::uint64_t at, const std::string &context,
                       bool syntax, Variables captured) -> std::int64_t {
                const auto &values = captured ? *captured : variables;
                return Expression(
                           expr,
                           [&](std::string_view key) -> std::int64_t {
                               if (key == "$")
                                   return static_cast<std::int64_t>(at);
                               const auto symbol = qualify(std::string(key), context);
                               if (symbol == "TRUE")
                                   return 1;
                               if (symbol == "FALSE")
                                   return 0;
                               if (auto it = values.find(symbol); it != values.end())
                                   return it->second;
                               if (auto it = labels.find(symbol); it != labels.end())
                                   return it->second;
                               auto it = constants.find(symbol);
                               if (it == constants.end())
                                   fail("unresolved symbol: " + symbol);
                               if (visiting.size() >= 128 || !visiting.insert(symbol).second)
                                   fail("cyclic constant: " + symbol);
                               const auto &def = it->second;
                               const auto value = eval(def.expression, def.pc, def.scope,
                                                       def.relaxed, def.variables);
                               visiting.erase(symbol);
                               return value;
                           },
                           syntax,
                           [&](std::string_view key, const std::vector<std::int64_t> &args) {
                               const auto it = functions.find(qualify(std::string(key), context));
                               if (it == functions.end())
                                   fail("unknown function: " + std::string(key));
                               const auto &fn = it->second;
                               if (args.size() != fn.parameters.size())
                                   fail("wrong function argument count");
                               if (++function_depth > 64)
                                   fail("function nesting limit exceeded");
                               const auto expanded = replace_symbols(
                                   fn.body.expression, [&](const std::string &token) {
                                       const auto p = std::find(fn.parameters.begin(),
                                                                fn.parameters.end(), upper(token));
                                       return p == fn.parameters.end()
                                                  ? token
                                                  : integer_text(args[static_cast<std::size_t>(
                                                        p - fn.parameters.begin())]);
                                   });
                               const auto result = eval(expanded, fn.body.pc, fn.body.scope,
                                                        fn.body.relaxed, captured);
                               --function_depth;
                               return result;
                           })
                    .value();
            };
            return eval(expression, address, local, mode, snapshot);
        };
        const auto freeze = [&](const std::string &arg) {
            return replace_symbols(arg, [&](const std::string &key) {
                const auto it = variables.find(qualify(key, scope));
                return it == variables.end() ? key : integer_text(it->second);
            });
        };
        const auto accept = [&](Line line) {
            active_line = line.number;
            active_file = line.file;
            active_trace = line.trace;
            line.address = pc;
            line.relaxed = relaxed;
            line.variables = variable_snapshot;
            const auto passive = [&] {
                line.listing_only = true;
                lines.push_back(std::move(line));
            };
            if (line.listing_only) {
                passive();
                return;
            }
            if (line.op == "FUNCTION") {
                if (!name(line.label) || line.args.size() < 2)
                    fail("FUNCTION needs a name, parameters and expression");
                const auto key = qualify(line.label, scope);
                if (!names.insert(key).second)
                    fail("duplicate function");
                Function fn;
                std::set<std::string> parameters;
                for (std::size_t i = 0; i + 1 < line.args.size(); ++i) {
                    const auto parameter = upper(line.args[i]);
                    if (!name(parameter) || parameter.find_first_of("._") != std::string::npos ||
                        !parameters.insert(parameter).second)
                        fail("invalid function parameter");
                    fn.parameters.push_back(parameter);
                }
                fn.body = {line.args.back(), scope, pc, relaxed, variable_snapshot};
                functions.emplace(key, std::move(fn));
                line.note = "function";
                passive();
                return;
            }
            const bool legacy_equ = line.op == ".EQU";
            if (line.op == "=" || legacy_equ)
                line.op = "EQU";
            if (line.op == "EQU" || line.op == "SET" || line.op == "EVAL" || line.op == ":=" ||
                line.op == "LABEL") {
                if (line.label.empty()) {
                    arity(line, 2);
                    line.label = upper(line.args[0]);
                    line.args.erase(line.args.begin());
                }
                arity(line, 1);
                if (!name(line.label))
                    fail("invalid symbol");
                if (legacy_equ && line.args[0].find('$') != std::string::npos)
                    fail("$ is not allowed in legacy .equ; use EQU");
                const auto key = qualify(line.label, scope);
                line.args[0] = freeze(line.args[0]);
                if (line.op == "SET" || line.op == "EVAL" || line.op == ":=") {
                    if (names.contains(key) && !variables.contains(key))
                        fail("cannot SET immutable symbol");
                    variables[key] = evaluate(line.args[0], pc, scope, relaxed);
                    line.note = "value=" + std::to_string(variables[key]);
                    variable_snapshot =
                        std::make_shared<const std::map<std::string, std::int64_t>>(variables);
                    names.insert(key);
                } else {
                    if (!names.insert(key).second)
                        fail("duplicate symbol: " + key);
                    if (line.op == "LABEL") {
                        labels[key] = evaluate(line.args[0], pc, scope, relaxed);
                        line.note = "value=" + std::to_string(labels[key]);
                    } else {
                        constants[key] = {line.args[0], scope, pc, relaxed, variable_snapshot};
                        line.symbol = key;
                    }
                }
                if (!line.label.starts_with('.'))
                    scope = key;
                passive();
                return;
            }
            if (!line.label.empty()) {
                const auto key = qualify(line.label, scope);
                if (!names.insert(key).second)
                    fail("duplicate symbol: " + key);
                labels[key] = static_cast<std::int64_t>(pc);
                if (!line.label.starts_with('.'))
                    scope = key;
            }
            line.scope = scope;
            for (std::size_t i = line.op == "BINCLUDE" ? 1 : 0; i < line.args.size(); ++i)
                line.args[i] = freeze(line.args[i]);
            const auto value = [&](std::size_t index) {
                return evaluate(line.args.at(index), pc, scope, relaxed);
            };
            if (line.op == "RELAXED") {
                arity(line, 1);
                const auto mode = upper(line.args[0]);
                if (mode != "ON" && mode != "OFF")
                    fail("RELAXED expects ON or OFF");
                relaxed = mode == "ON";
                passive();
                return;
            }
            if (line.op == "CPU") {
                arity(line, 1);
                if (upper(line.args[0]) != "YOSIC-KAPPA")
                    fail("unsupported CPU");
                passive();
                return;
            }
            if (line.op.empty()) {
                passive();
                return;
            }
            if (line.op == "ORG" || line.op == ".ORG") {
                arity(line, 1);
                const auto target = value(0);
                if (target < base || std::uint64_t(target) > std::uint64_t(base) + 65536)
                    fail("ORG outside image");
                pc = static_cast<std::uint64_t>(target);
                line.note = "ORG -> " + hex_address(pc);
                passive();
                return;
            }
            if (line.op == "BINCLUDE") {
                if (line.args.empty() || line.args.size() > 3 ||
                    std::any_of(line.args.begin(), line.args.end(),
                                [](const auto &a) { return a.empty(); }))
                    fail("BINCLUDE expects file[,offset[,length]]");
                const auto path = files.resolve(line, false);
                const auto &data = files.read(path);
                const auto offset = line.args.size() > 1 ? value(1) : 0;
                if (offset < 0 || static_cast<std::uint64_t>(offset) > data.size())
                    fail("BINCLUDE range outside file");
                const auto length = line.args.size() > 2
                                        ? value(2)
                                        : static_cast<std::int64_t>(data.size()) - offset;
                if (offset < 0 || static_cast<std::uint64_t>(offset) > data.size() || length < 0 ||
                    static_cast<std::uint64_t>(length) >
                        data.size() - static_cast<std::size_t>(offset))
                    fail("BINCLUDE range outside file");
                if (static_cast<std::uint64_t>(length) > std::uint64_t(base) + 65536 - pc)
                    fail("image exceeds 64 KiB");
                const auto first = data.begin() + static_cast<std::ptrdiff_t>(offset);
                line.binary.assign(first, first + static_cast<std::ptrdiff_t>(length));
                line.size = line.binary.size();
                line.note = "BINCLUDE " + files.display(path) +
                            " offset=" + std::to_string(offset) +
                            " length=" + std::to_string(length);
            } else if (line.op == "ALIGN" || line.op == ".ALIGN") {
                if (line.args.size() != 1 && line.args.size() != 2)
                    fail("ALIGN expects boundary[,fill]");
                const auto align = value(0);
                if (align <= 0 || align > 65536 || (line.op == ".ALIGN" && (align & (align - 1))))
                    fail("invalid alignment");
                line.size =
                    (std::uint64_t(align) - pc % std::uint64_t(align)) % std::uint64_t(align);
            } else if (line.op == ".SPACE" || line.op == "DS.B" || line.op == "DS.W" ||
                       line.op == "DS.L") {
                arity(line, 1);
                const auto size = value(0);
                const unsigned unit = line.op == "DS.W" ? 2 : line.op == "DS.L" ? 4 : 1;
                if (size < 0 || size > 65536 / unit)
                    fail("invalid space size");
                line.size = static_cast<std::uint64_t>(size) * unit;
                line.op = ".SPACE";
            } else if (line.op == ".ASCII") {
                arity(line, 1);
                line.size = literal(line.args[0]).size();
            } else if (line.op == ".BYTE" || line.op == ".WORD" || line.op == "DB" ||
                       line.op == "DW" || line.op == "DD") {
                if (line.args.empty())
                    fail("data directive needs operands");
                if (line.op == ".WORD" && (pc & 3u))
                    fail("unaligned legacy word");
                for (const auto &arg : line.args) {
                    if (arg.empty())
                        fail("empty operand");
                    if (line.op == "DB" && quoted_string(arg))
                        line.size += literal(arg).size();
                    else
                        line.size += line.op == "DW"                         ? 2
                                     : line.op == "DD" || line.op == ".WORD" ? 4
                                                                             : 1;
                }
            } else {
                if (!opcodes.contains(line.op))
                    fail("unknown instruction: " + line.op);
                if (pc & 3u)
                    fail("unaligned instruction");
                line.size = 4;
            }
            if (line.size > std::uint64_t(base) + 65536 - pc)
                fail("image exceeds 64 KiB");
            pc += line.size;
            lines.push_back(std::move(line));
        };
        std::vector<std::string> scopes;
        MacroProcessor processor(
            files, accept,
            [&](const std::string &expr) { return evaluate(expr, pc, scope, relaxed); },
            [&](const std::string &key) { return names.contains(qualify(key, scope)); },
            [&](bool enter) {
                if (enter)
                    scopes.push_back(scope);
                else {
                    scope = scopes.back();
                    scopes.pop_back();
                }
            });
        processor.process(source, options.source_path);
        Assembly result;
        std::vector<bool> occupied(65536, false);
        std::ostringstream listing, map;
        listing << "YOSIC-KAPPA Assembler Listing v1\n"
                << "    Line M   Address Object bytes            Source\n";
        for (auto &line : lines) {
            active_line = line.number;
            active_file = line.file;
            active_trace = line.trace;
            if (line.listing_only) {
                if (!line.symbol.empty())
                    line.note = "value=" + std::to_string(evaluate(line.symbol, base, "", false));
                listing_line(listing, line, {});
                continue;
            }
            std::vector<std::uint8_t> bytes;
            const auto emit_word = [&](std::uint32_t word) {
                for (unsigned i = 0; i < 4; ++i)
                    bytes.push_back(static_cast<std::uint8_t>(word >> (8 * i)));
            };
            if (line.op == "BINCLUDE")
                bytes = line.binary;
            else if (line.op == ".SPACE" || line.op == ".ALIGN" || line.op == "ALIGN") {
                const auto fill = line.op != ".SPACE" && line.args.size() == 2
                                      ? evaluate(line.args[1], line.address, line.scope,
                                                 line.relaxed, line.variables)
                                      : 0;
                if (fill < -128 || fill > 255)
                    fail("ALIGN fill out of range");
                bytes.resize(static_cast<std::size_t>(line.size), static_cast<std::uint8_t>(fill));
            } else if (line.op == ".ASCII")
                bytes = literal(line.args[0]);
            else if (line.op == ".BYTE" || line.op == ".WORD" || line.op == "DB" ||
                     line.op == "DW" || line.op == "DD") {
                for (const auto &arg : line.args) {
                    if (line.op == "DB" && quoted_string(arg)) {
                        const auto data = literal(arg);
                        bytes.insert(bytes.end(), data.begin(), data.end());
                        continue;
                    }
                    const auto value =
                        evaluate(arg, line.address, line.scope, line.relaxed, line.variables);
                    if (line.op == ".BYTE" || line.op == "DB") {
                        if (value < -128 || value > 255)
                            fail("byte out of range");
                        bytes.push_back(static_cast<std::uint8_t>(value));
                    } else if (line.op == "DW") {
                        if (value < -32768 || value > 65535)
                            fail("16-bit word out of range");
                        bytes.push_back(static_cast<std::uint8_t>(value));
                        bytes.push_back(
                            static_cast<std::uint8_t>(static_cast<std::uint64_t>(value) >> 8));
                    } else {
                        if (value < -2147483648ll || value > 0xffffffffll)
                            fail("word out of range");
                        emit_word(static_cast<std::uint32_t>(value));
                    }
                }
            } else {
                const unsigned op = opcodes.at(line.op);
                std::uint32_t word = op << 26;
                const auto immediate = [&](const std::string &text) {
                    const auto value =
                        evaluate(text, line.address, line.scope, line.relaxed, line.variables);
                    if (value < -32768 || value > 32767)
                        fail("immediate out of signed 16-bit range");
                    return static_cast<std::uint32_t>(value) & 0xffffu;
                };
                if (op == 0)
                    arity(line, 0);
                else if (op == 6) {
                    arity(line, 1);
                    const auto target = evaluate(line.args[0], line.address, line.scope,
                                                 line.relaxed, line.variables);
                    if (target < base || std::uint64_t(target) >= std::uint64_t(base) + 65536 ||
                        (target & 3))
                        fail("jump outside segment or unaligned");
                    const auto delta = (target - static_cast<std::int64_t>(line.address)) / 4;
                    word |= static_cast<std::uint32_t>(delta) & 0x1fffffu;
                } else if (op == 1 || op == 2) {
                    arity(line, 2);
                    const auto &operand = line.args[1];
                    if (operand.size() < 3 || operand.front() != '[' || operand.back() != ']')
                        fail("expected [register+offset]");
                    const auto inside =
                        trim(std::string_view(operand).substr(1, operand.size() - 2));
                    const auto split = inside.find_first_of("+- \t");
                    const auto rb = reg(inside.substr(0, split));
                    const auto offset = split == std::string::npos
                                            ? std::string{}
                                            : trim(std::string_view(inside).substr(split));
                    if (!offset.empty() && offset[0] != '+' && offset[0] != '-')
                        fail("expected signed memory offset");
                    word |= reg(line.args[0]) << 21;
                    word |= rb << 16;
                    if (!offset.empty())
                        word |= immediate(offset);
                } else if (op == 3 || op == 4 || op == 5 || op == 8) {
                    arity(line, 1);
                    word |= reg(line.args[0]) << 21;
                } else if (op == 0x20) {
                    arity(line, 2);
                    const auto seg = upper(line.args[0]);
                    const std::map<std::string, unsigned> segments{
                        {"CS", 0}, {"DS", 1}, {"SS", 2}, {"ES", 3}};
                    if (!segments.contains(seg))
                        fail("invalid segment register");
                    word |= segments.at(seg) << 21;
                    word |= reg(line.args[1]) << 16;
                } else {
                    arity(line, 3);
                    word |= reg(line.args[0]) << 21;
                    word |= reg(line.args[1]) << 16;
                    word |= op == 0x11 ? immediate(line.args[2]) : reg(line.args[2]) << 11;
                }
                emit_word(word);
            }
            const auto offset = static_cast<std::size_t>(line.address - base);
            if (bytes.size() != line.size)
                fail("internal layout mismatch");
            for (std::size_t i = 0; i < bytes.size(); ++i) {
                if (occupied[offset + i])
                    fail("overlapping output");
                occupied[offset + i] = true;
            }
            if (!bytes.empty()) {
                result.bytes.resize(std::max(result.bytes.size(), offset + bytes.size()), 0);
                std::copy(bytes.begin(), bytes.end(),
                          result.bytes.begin() + static_cast<std::ptrdiff_t>(offset));
            }
            listing_line(listing, line, bytes);
        }
        for (std::size_t i = 0; i < result.bytes.size();) {
            if (occupied[i]) {
                ++i;
                continue;
            }
            const auto start = i;
            while (i < result.bytes.size() && !occupied[i])
                ++i;
            listing << "; zero-fill " << hex_address(base + start) << ".."
                    << hex_address(base + i - 1) << " length=" << (i - start) << '\n';
        }
        for (const auto &[key, value] : labels)
            map << key << " = 0x" << std::hex << value << '\n';
        for (const auto &[key, expression] : constants) {
            (void)expression;
            map << key << " = " << std::dec << evaluate(key, base, "", false) << '\n';
        }
        result.listing = listing.str();
        result.map = map.str();
        result.dependencies = std::move(files.dependencies);
        return result;
    } catch (const std::invalid_argument &error) {
        throw std::invalid_argument(
            "line " + std::to_string(active_line) +
            (active_file.empty() ? ""
                                 : " (" + active_file + ":" + std::to_string(active_line) +
                                       (active_trace.empty() ? "" : " via " + active_trace) + ")") +
            ": " + error.what());
    }
}
} // namespace yosic

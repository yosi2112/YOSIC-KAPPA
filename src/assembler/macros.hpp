#pragma once
#include "source.hpp"
#include <map>
#include <sstream>
#include <utility>
namespace yosic::as_syntax {
class MacroProcessor {
    struct Parameter {
        std::string name, fallback;
    };
    struct Macro {
        std::vector<Parameter> parameters;
        std::vector<SourceLine> body;
        bool globals = false, intlabel = false;
    };
    struct Frame {
        std::vector<Parameter> parameters;
        std::vector<std::string> actual;
        std::string attribute, label;
        bool intlabel = false;
        std::map<std::string, std::string> replacements() const {
            std::map<std::string, std::string> result;
            for (std::size_t i = 0; i < parameters.size(); ++i)
                result[parameters[i].name] = i < actual.size() ? actual[i] : "";
            std::string all;
            for (std::size_t i = 0; i < actual.size(); ++i)
                all += (i ? "," : "") + actual[i];
            result["ALLARGS"] = all;
            result["ARGCOUNT"] = std::to_string(actual.size());
            result["ATTRIBUTE"] = attribute;
            if (intlabel)
                result["__LABEL__"] = label;
            return result;
        }
    };
    SourceFiles &files_;
    std::vector<std::filesystem::path> includes_;
    std::map<std::string, Macro> macros_;
    std::function<void(Line)> emit_;
    std::function<std::int64_t(const std::string &)> eval_;
    std::function<bool(const std::string &)> defined_;
    std::function<void(bool)> scope_;
    std::size_t serial_ = 0, work_ = 0, text_size_ = 0;
    unsigned maxnest_ = 64;
    bool ended_ = false;
    static bool parameter_name(const std::string &s) {
        return !s.empty() && std::isalpha(static_cast<unsigned char>(s[0])) &&
               std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isalnum(c); });
    }
    static bool block(const std::string &op) {
        return op == "MACRO" || op == "REPT" || op == "IRP" || op == "IRPC" || op == "WHILE";
    }
    static bool conditional(const std::string &op) {
        return op == "IF" || op == "IFDEF" || op == "IFNDEF";
    }
    static std::string substitute(const std::string &text,
                                  const std::map<std::string, std::string> &values) {
        std::string out;
        char quote = 0;
        bool escape = false;
        for (std::size_t i = 0; i < text.size();) {
            char c = text[i];
            if (escape) {
                out += c;
                ++i;
                escape = false;
                continue;
            }
            if (quote && c == '\\') {
                out += c;
                ++i;
                escape = true;
                continue;
            }
            if (c == '\'' || c == '"') {
                if (!quote)
                    quote = c;
                else if (quote == c)
                    quote = 0;
                out += c;
                ++i;
                continue;
            }
            if (!quote && c == ';') {
                out += text.substr(i);
                break;
            }
            if (text.substr(i, 9) == "__LABEL__" || upper(text.substr(i, 9)) == "__LABEL__") {
                auto it = values.find("__LABEL__");
                if (it != values.end()) {
                    out += it->second;
                    i += 9;
                    continue;
                }
            }
            if (std::isalnum(static_cast<unsigned char>(c))) {
                const auto start = i++;
                while (i < text.size() && std::isalnum(static_cast<unsigned char>(text[i])))
                    ++i;
                const auto token = text.substr(start, i - start);
                const auto it = values.find(quote ? token : upper(token));
                out += it == values.end() ? token : it->second;
            } else {
                out += c;
                ++i;
            }
            if (out.size() > 1024 * 1024)
                error("macro line exceeds 1 MiB");
        }
        return out;
    }
    bool condition(const Line &line) {
        arity(line, 1);
        if (line.op == "IFDEF")
            return defined_(upper(line.args[0]));
        if (line.op == "IFNDEF")
            return !defined_(upper(line.args[0]));
        // AS macros commonly test a substituted argument against an empty string.
        const auto &text = line.args[0];
        char quote = 0;
        bool escape = false;
        for (std::size_t i = 0; i < text.size(); ++i) {
            const char c = text[i];
            if (escape) {
                escape = false;
                continue;
            }
            if (quote) {
                if (c == '\\')
                    escape = true;
                else if (c == quote)
                    quote = 0;
                continue;
            }
            if (c == '\'' || c == '"') {
                quote = c;
                continue;
            }
            if (c == '=' || c == '<' || c == '>' ||
                (c == '!' && i + 1 < text.size() && text[i + 1] == '=')) {
                const auto left = trim(std::string_view(text).substr(0, i));
                auto width = (i + 1 < text.size() &&
                              (text[i + 1] == '=' || (c == '<' && text[i + 1] == '>')))
                                 ? 2u
                                 : 1u;
                const auto right = trim(std::string_view(text).substr(i + width));
                if (quoted_string(left) && quoted_string(right)) {
                    const auto a = literal(left), b = literal(right);
                    const auto op = text.substr(i, width);
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
                    return a >= b;
                }
                break;
            }
        }
        return eval_(text) != 0;
    }
    static std::pair<std::vector<SourceLine>, std::size_t>
    body(const std::vector<SourceLine> &source, std::size_t start) {
        unsigned nesting = 1;
        std::vector<SourceLine> result;
        for (auto i = start + 1; i < source.size(); ++i) {
            if (source[i].boundary)
                error("missing ENDM in include file");
            const auto parsed = parse(source[i].text, source[i].number);
            const auto &op = parsed.op;
            if (block(op))
                ++nesting;
            else if (op == "ENDM") {
                arity(parsed, 0);
                if (--nesting == 0)
                    return {result, i};
            }
            result.push_back(source[i]);
        }
        error("missing ENDM");
    }
    static bool options(std::vector<std::string> &args, bool &intlabel) {
        bool globals = false;
        std::vector<std::string> operands;
        for (const auto &arg : args) {
            if (arg.starts_with('{')) {
                const auto option = upper(arg);
                if (option == "{GLOBALSYMBOLS}")
                    globals = true;
                else if (option == "{NOGLOBALSYMBOLS}")
                    globals = false;
                else if (option == "{INTLABEL}")
                    intlabel = true;
                else if (option == "{NOINTLABEL}")
                    intlabel = false;
                else
                    error("unsupported macro control parameter");
            } else
                operands.push_back(arg);
        }
        args = std::move(operands);
        return globals;
    }
    static Frame bind(const Macro &macro, const Line &call, const std::string &attribute) {
        Frame frame;
        frame.parameters = macro.parameters;
        frame.attribute = attribute;
        frame.label = call.label;
        frame.intlabel = macro.intlabel;
        frame.actual.resize(macro.parameters.size());
        for (std::size_t i = 0; i < frame.actual.size(); ++i)
            frame.actual[i] = macro.parameters[i].fallback;
        bool keywords = false;
        std::vector<bool> assigned(macro.parameters.size(), false);
        for (std::size_t i = 0; i < call.args.size(); ++i) {
            const auto &arg = call.args[i];
            const auto equal = arg.find('=');
            if (equal != std::string::npos && parameter_name(trim(arg.substr(0, equal)))) {
                keywords = true;
                const auto key = upper(trim(arg.substr(0, equal)));
                const auto it = std::find_if(macro.parameters.begin(), macro.parameters.end(),
                                             [&](const auto &p) { return p.name == key; });
                if (it == macro.parameters.end())
                    error("unknown macro keyword parameter");
                const auto at = static_cast<std::size_t>(it - macro.parameters.begin());
                if (assigned[at])
                    error("duplicate macro argument");
                assigned[at] = true;
                frame.actual[at] = trim(arg.substr(equal + 1));
            } else {
                if (keywords)
                    error("positional argument follows keyword argument");
                if (i >= frame.actual.size())
                    frame.actual.push_back(arg);
                else {
                    if (!arg.empty())
                        frame.actual[i] = arg;
                    assigned[i] = true;
                }
            }
        }
        return frame;
    }
    bool expand_local(const std::vector<SourceLine> &source, Frame *frame, bool globals,
                      unsigned depth, const std::string &trace, bool prepared = false) {
        std::map<std::string, std::string> locals;
        if (!globals) {
            const auto prefix = "__M" + std::to_string(++serial_) + "_";
            for (const auto &item : source) {
                const auto text =
                    frame && !prepared ? substitute(item.text, frame->replacements()) : item.text;
                auto line = parse(text, item.number);
                if (line.label.empty() && (line.op == "EQU" || line.op == ".EQU") &&
                    line.args.size() == 2)
                    line.label = upper(line.args[0]);
                const bool existing_variable =
                    (line.op == "SET" || line.op == "EVAL" || line.op == ":=") &&
                    defined_(line.label);
                if (!line.label.empty() && line.op != "MACRO" && line.op != "LABEL" &&
                    !existing_variable &&
                    line.label != (frame && frame->intlabel ? frame->label : ""))
                    locals[line.label] = prefix + line.label;
            }
        }
        scope_(true);
        try {
            const bool exit = run(source, frame, locals, depth, trace, prepared);
            scope_(false);
            return exit;
        } catch (...) {
            scope_(false);
            throw;
        }
    }
    bool run(const std::vector<SourceLine> &input_source, Frame *frame,
             const std::map<std::string, std::string> &locals, unsigned depth,
             const std::string &inherited_trace, bool prepared = false) {
        auto source = input_source;
        struct IncludeScope {
            std::vector<std::filesystem::path> &stack;
            std::size_t size;
            ~IncludeScope() { stack.resize(size); }
        } include_scope{includes_, includes_.size()};
        if (depth > maxnest_)
            error("macro nesting limit exceeded");
        struct Branch {
            bool parent, taken, active, otherwise;
        };
        std::vector<Branch> branches;
        const auto active = [&] { return branches.empty() || branches.back().active; };
        const auto observe = [&](Line line, char mark) {
            line.listing_only = true;
            line.mark = mark;
            emit_(std::move(line));
        };
        const auto raw_line = [&](const SourceLine &item, char mark) {
            if (item.boundary)
                return;
            Line raw;
            raw.number = item.number;
            raw.original = item.text;
            raw.file = item.file;
            raw.path = item.path;
            raw.trace = item.route.value_or(inherited_trace);
            observe(std::move(raw), mark);
        };
        const auto tail = [&](std::size_t start) {
            for (auto n = start; n < source.size(); ++n)
                raw_line(source[n], '-');
        };
        for (std::size_t i = 0; i < source.size() && !ended_; ++i) {
            if (source[i].boundary) {
                if (branches.size() != source[i].branch_floor)
                    error("missing ENDIF in include file");
                includes_.pop_back();
                continue;
            }
            const auto trace = source[i].route.value_or(inherited_trace);
            if (++work_ > 100000)
                error("macro expansion work limit exceeded");
            auto text = frame && !prepared ? substitute(source[i].text, frame->replacements())
                                           : source[i].text;
            text = replace_symbols(text, [&](const std::string &token) {
                const auto it = locals.find(upper(token));
                return it == locals.end() ? token : it->second;
            });
            text_size_ += text.size();
            if (text_size_ > 16 * 1024 * 1024)
                error("macro expanded text exceeds 16 MiB");
            const auto current = source[i];
            try {
                auto line = parse(text, current.number);
                line.original = text;
                line.file = current.file;
                line.path = current.path;
                line.trace = trace;
                line.mark = depth ? 'E' : ' ';
                const auto route = [&](const std::string &kind, std::size_t iteration) {
                    return trace + (trace.empty() ? "" : " > ") + line.file + ":" +
                           std::to_string(line.number) + "/" + kind + "#" +
                           std::to_string(iteration);
                };
                if (conditional(line.op)) {
                    observe(line, 'C');
                    const bool parent = active();
                    const bool enabled = parent && condition(line);
                    branches.push_back({parent, enabled, enabled, false});
                    continue;
                }
                if (line.op == "ELSEIF" || line.op == "ELSE") {
                    observe(line, 'C');
                    if (branches.size() <= source[i].branch_floor || branches.back().otherwise)
                        error("misplaced ELSEIF/ELSE");
                    auto &branch = branches.back();
                    const bool otherwise = line.op == "ELSE" || line.args.empty();
                    if (line.op == "ELSE")
                        arity(line, 0);
                    branch.active =
                        branch.parent && !branch.taken && (otherwise || condition(line));
                    branch.taken = branch.taken || branch.active;
                    branch.otherwise = otherwise;
                    continue;
                }
                if (line.op == "ENDIF") {
                    observe(line, 'C');
                    arity(line, 0);
                    if (branches.size() <= source[i].branch_floor)
                        error("unmatched ENDIF");
                    branches.pop_back();
                    continue;
                }
                if (block(line.op)) {
                    auto [block_body, last] = body(source, i);
                    observe(line, !active() ? '-' : line.op == "MACRO" ? 'D' : 'C');
                    for (auto n = i + 1; n <= last; ++n)
                        raw_line(source[n], active() ? 'D' : '-');
                    i = last;
                    if (!active())
                        continue;
                    // The enclosing macro substitutes a captured block once (SHIFT follows AS
                    // rules).
                    for (auto &item : block_body) {
                        item.route.reset();
                        item.branch_floor = 0;
                        if (frame && !prepared)
                            item.text = substitute(item.text, frame->replacements());
                        item.text = replace_symbols(item.text, [&](const std::string &token) {
                            auto it = locals.find(upper(token));
                            return it == locals.end() ? token : it->second;
                        });
                    }
                    bool intlabel = false;
                    const bool globals = options(line.args, intlabel);
                    if (line.op == "MACRO") {
                        if (!name(line.label) || macros_.contains(line.label))
                            error("invalid or duplicate macro name");
                        Macro macro;
                        macro.body = std::move(block_body);
                        macro.globals = globals;
                        macro.intlabel = intlabel;
                        std::map<std::string, bool> seen;
                        for (const auto &arg : line.args) {
                            const auto split = arg.find('=');
                            const auto key = upper(trim(arg.substr(0, split)));
                            if (!parameter_name(key) || key == "ALLARGS" || key == "ARGCOUNT" ||
                                key == "ATTRIBUTE" || !seen.emplace(key, true).second)
                                error("invalid macro parameter");
                            macro.parameters.push_back({key, split == std::string::npos
                                                                 ? ""
                                                                 : trim(arg.substr(split + 1))});
                        }
                        macros_.emplace(line.label, std::move(macro));
                        continue;
                    }
                    if (!line.label.empty()) {
                        Line label = line;
                        label.op.clear();
                        label.args.clear();
                        label.hidden = true;
                        emit_(label);
                    }
                    if (intlabel)
                        error("INTLABEL only applies to MACRO");
                    if (line.op == "REPT" || line.op == "WHILE") {
                        arity(line, 1);
                        const auto count = line.op == "REPT" ? eval_(line.args[0]) : 0;
                        if (count > 100000)
                            error("repeat count exceeds limit");
                        for (std::int64_t n = 0; line.op == "REPT" ? n < count : condition(line);
                             ++n) {
                            if (++work_ > 100000)
                                error("macro expansion work limit exceeded");
                            if (expand_local(block_body, frame, globals, depth + 1,
                                             route(line.op, static_cast<std::size_t>(n + 1)),
                                             true) ||
                                ended_)
                                break;
                        }
                    } else {
                        if (line.args.size() < 2 || !parameter_name(line.args[0]))
                            error("IRP/IRPC needs parameter and values");
                        std::vector<std::string> values;
                        if (line.op == "IRPC") {
                            arity(line, 2);
                            for (auto ch : literal(line.args[1]))
                                values.emplace_back(1, static_cast<char>(ch));
                        } else
                            values.assign(line.args.begin() + 1, line.args.end());
                        std::size_t iteration_number = 0;
                        for (const auto &value : values) {
                            auto iteration = block_body;
                            for (auto &item : iteration)
                                item.text = substitute(item.text, {{upper(line.args[0]), value}});
                            if (expand_local(iteration, frame, globals, depth + 1,
                                             route(line.op, ++iteration_number), true) ||
                                ended_)
                                break;
                        }
                    }
                    if (ended_) {
                        tail(i + 1);
                        return false;
                    }
                    continue;
                }
                if (!active()) {
                    observe(line, '-');
                    continue;
                }
                if (line.op == "INCLUDE") {
                    arity(line, 1);
                    const auto path = files_.resolve(line, true);
                    if (includes_.size() >= 64)
                        error("include nesting limit exceeded");
                    for (const auto &parent : includes_)
                        if (same_file(parent, path))
                            throw std::invalid_argument("cyclic INCLUDE: " + files_.display(path));
                    observe(line, 'C');
                    if (!line.label.empty()) {
                        auto label = line;
                        label.op.clear();
                        label.original.clear();
                        label.hidden = true;
                        label.args.clear();
                        emit_(std::move(label));
                    }
                    auto included = files_.lines(files_.read(path), path);
                    for (auto &item : included) {
                        item.route = route("INCLUDE", 1);
                        item.branch_floor = branches.size();
                    }
                    SourceLine end{"",    line.number, line.path,      line.file,
                                   trace, true,        branches.size()};
                    included.push_back(std::move(end));
                    includes_.push_back(path);
                    source.insert(source.begin() + static_cast<std::ptrdiff_t>(i + 1),
                                  std::make_move_iterator(included.begin()),
                                  std::make_move_iterator(included.end()));
                    continue;
                }
                if (line.op == "ENDM")
                    error("unmatched ENDM");
                if (line.op == "EXITM") {
                    observe(line, 'C');
                    arity(line, 0);
                    if (!depth)
                        error("EXITM outside expansion");
                    tail(i + 1);
                    return true;
                }
                if (line.op == "SHIFT") {
                    observe(line, 'C');
                    arity(line, 0);
                    if (!frame)
                        error("SHIFT outside macro");
                    if (!frame->actual.empty())
                        frame->actual.erase(frame->actual.begin());
                    continue;
                }
                if (line.op == "MAXNEST") {
                    observe(line, 'C');
                    arity(line, 1);
                    const auto limit = eval_(line.args[0]);
                    if (limit < 1 || limit > 256)
                        error("MAXNEST must be 1..256");
                    maxnest_ = static_cast<unsigned>(limit);
                    continue;
                }
                if (line.op == "END") {
                    observe(line, 'C');
                    arity(line, 0);
                    ended_ = true;
                    tail(i + 1);
                    return false;
                }
                bool direct = line.op.starts_with('!');
                if (direct)
                    line.op.erase(0, 1);
                auto macro = macros_.find(line.op);
                std::string attribute;
                if (!direct && macro == macros_.end()) {
                    const auto dot = line.op.find('.');
                    if (dot != std::string::npos) {
                        macro = macros_.find(line.op.substr(0, dot));
                        attribute = line.op.substr(dot + 1);
                    }
                }
                if (!direct && macro != macros_.end()) {
                    observe(line, 'C');
                    const auto definition = macro->second;
                    auto arguments = bind(definition, line, attribute);
                    if (!line.label.empty() && !definition.intlabel) {
                        Line label = line;
                        label.op.clear();
                        label.args.clear();
                        label.hidden = true;
                        emit_(label);
                    }
                    (void)expand_local(definition.body, &arguments, definition.globals, depth + 1,
                                       route(line.op, 1));
                } else
                    emit_(std::move(line));
                if (ended_) {
                    tail(i + 1);
                    return false;
                }
            } catch (const std::invalid_argument &e) {
                throw std::invalid_argument("source line " + std::to_string(current.number) + " (" +
                                            current.file + ":" + std::to_string(current.number) +
                                            (trace.empty() ? "" : " via " + trace) +
                                            "): " + e.what());
            } catch (const std::runtime_error &e) {
                throw std::runtime_error(current.file + ":" + std::to_string(current.number) +
                                         (trace.empty() ? "" : " via " + trace) + ": " + e.what());
            }
        }
        if (!branches.empty())
            error("missing ENDIF");
        return false;
    }

  public:
    MacroProcessor(SourceFiles &files, std::function<void(Line)> emit,
                   std::function<std::int64_t(const std::string &)> eval,
                   std::function<bool(const std::string &)> defined,
                   std::function<void(bool)> scope)
        : files_(files), emit_(std::move(emit)), eval_(std::move(eval)),
          defined_(std::move(defined)), scope_(std::move(scope)) {}
    void process(std::string_view text, const std::filesystem::path &path) {
        files_.root(text);
        if (!path.empty())
            includes_.push_back(std::filesystem::weakly_canonical(path));
        (void)run(files_.lines(text, path.empty() ? path : std::filesystem::absolute(path)),
                  nullptr, {}, 0, "");
    }
};
} // namespace yosic::as_syntax

#include <scc/as/align.hpp>
#include <scc/as/data.hpp>
#include <scc/as/fill.hpp>
#include <scc/as/instruction.hpp>
#include <scc/as/module.hpp>
#include <scc/as/operand.hpp>
#include <scc/as/parser.hpp>

#include <scc/platform.hpp>

#include <istream>
#include <memory>
#include <vector>

static bool isdigit(const int c, const int base)
{
    switch (base)
    {
    case 8:
        return '0' <= c && c <= '7';
    case 10:
        return isdigit(c);
    case 16:
        return isxdigit(c);
    default:
        return false;
    }
}

scc::as::Parser::Parser(std::istream &stream, Module &module)
    : m_Stream(stream),
      m_Platform(module.GetPlatform()),
      m_Module(module),
      m_Section(module.GetOrCreateSection(".text")),
      m_Primary()
{
    m_Buffer = m_Stream.get();
    m_Token = Get();
}

scc::as::Token scc::as::Parser::Get()
{
    enum class State
    {
        None,
        Comment,
        Immediate,
        Character,
        String,
        Symbol,
    };

    std::string raw;
    std::string value;

    auto state = State::None;
    auto base = 0;
    auto sign = false;
    auto reg = false;

    while (m_Buffer >= 0)
    {
        switch (state)
        {
        case State::None:
            switch (m_Buffer)
            {
            case '\n':
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                return { .Type = TokenType::EndOfLine, .Raw = std::move(raw) };

            case '#':
                state = State::Comment;
                break;

            case '-':
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                sign = true;
                break;

            case '0':
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                if (m_Buffer == 'x')
                {
                    raw += static_cast<char>(m_Buffer);
                    m_Buffer = m_Stream.get();
                    base = 16;
                }
                else
                {
                    value += '0';
                    base = 8;
                }
                state = State::Immediate;
                break;

            case '%':
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                reg = true;
                state = State::Symbol;
                break;

            case '\'':
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                state = State::Character;
                break;

            case '"':
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                state = State::String;
                break;

            default:
                if (std::isspace(m_Buffer))
                {
                    raw += static_cast<char>(m_Buffer);
                    m_Buffer = m_Stream.get();
                    break;
                }

                if (std::isdigit(m_Buffer))
                {
                    base = 10;
                    state = State::Immediate;
                    break;
                }

                if (std::isalpha(m_Buffer) || m_Buffer == '_' || m_Buffer == '.')
                {
                    state = State::Symbol;
                    break;
                }

                raw += static_cast<char>(m_Buffer);
                value += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                return { .Type = TokenType::Other, .Raw = std::move(raw), .Value = std::move(value) };
            }
            break;

        case State::Comment:
            if (m_Buffer == '\n')
            {
                state = State::None;
                break;
            }
            raw += static_cast<char>(m_Buffer);
            m_Buffer = m_Stream.get();
            break;

        case State::Immediate:
            if (!isdigit(m_Buffer, base))
            {
                const auto immediate = sign
                                           ? static_cast<Immediate>(std::stoll(value, {}, base))
                                           : std::stoull(value, {}, base);
                return {
                    .Type = TokenType::Immediate,
                    .Raw = std::move(raw),
                    .Value = std::move(value),
                    .Immediate = immediate,
                };
            }

            raw += static_cast<char>(m_Buffer);
            value += static_cast<char>(m_Buffer);
            m_Buffer = m_Stream.get();
            break;

        case State::Character:
            if (m_Buffer == '\'')
            {
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                const auto immediate = static_cast<unsigned char>(value.front());
                return {
                    .Type = TokenType::Immediate,
                    .Raw = std::move(raw),
                    .Value = std::move(value),
                    .Immediate = immediate,
                };
            }

            raw += static_cast<char>(m_Buffer);
            value += static_cast<char>(m_Buffer);
            m_Buffer = m_Stream.get();
            break;

        case State::String:
            if (m_Buffer == '"')
            {
                raw += static_cast<char>(m_Buffer);
                m_Buffer = m_Stream.get();
                return {
                    .Type = TokenType::String,
                    .Raw = std::move(raw),
                    .Value = std::move(value),
                };
            }

            if (m_Buffer == '\\')
            {
                m_Buffer = m_Stream.get();

                switch (m_Buffer)
                {
                case 'a':
                    m_Buffer = '\a';
                    break;
                case 'b':
                    m_Buffer = '\b';
                    break;
                case 'f':
                    m_Buffer = '\f';
                    break;
                case 'n':
                    m_Buffer = '\n';
                    break;
                case 'r':
                    m_Buffer = '\r';
                    break;
                case 't':
                    m_Buffer = '\t';
                    break;
                case 'v':
                    m_Buffer = '\v';
                    break;

                default:
                    break;
                }
            }

            raw += static_cast<char>(m_Buffer);
            value += static_cast<char>(m_Buffer);
            m_Buffer = m_Stream.get();
            break;

        case State::Symbol:
            if (!std::isalnum(m_Buffer) && m_Buffer != '_' && m_Buffer != '.')
            {
                if (reg)
                    return { .Type = TokenType::Register, .Raw = std::move(raw), .Value = std::move(value) };

                if (m_Buffer == ':')
                {
                    raw += static_cast<char>(m_Buffer);
                    m_Buffer = m_Stream.get();
                    return { .Type = TokenType::Label, .Raw = std::move(raw), .Value = std::move(value) };
                }

                return { .Type = TokenType::Symbol, .Raw = std::move(raw), .Value = std::move(value) };
            }

            raw += static_cast<char>(m_Buffer);
            value += static_cast<char>(m_Buffer);
            m_Buffer = m_Stream.get();
            break;
        }
    }

    return { .Type = TokenType::EndOfFile };
}

scc::as::Token &scc::as::Parser::Next()
{
    return m_Token = Get();
}

scc::as::Token scc::as::Parser::Skip()
{
    auto t = m_Token;
    m_Token = Get();
    return t;
}

bool scc::as::Parser::At(const TokenType type, const std::string &value) const
{
    if (m_Token.Type != type)
        return false;

    if (!value.empty() && m_Token.Value != value)
        return false;

    return true;
}

bool scc::as::Parser::Skip(const TokenType type, const std::string &value)
{
    if (!At(type, value))
        return false;

    Skip();
    return true;
}

toolkit::result<scc::as::Token> scc::as::Parser::Expect(const TokenType type, const std::string &value)
{
    if (m_Token.Type != type)
        return toolkit::make_error("expected type {}, got {}", type, m_Token.Type);

    if (!value.empty() && m_Token.Value != value)
        return toolkit::make_error("expected value '{}', got '{}'", value, m_Token.Value);

    return Skip();
}

void scc::as::Parser::Parse()
{
    while (m_Token.Type != TokenType::EndOfFile)
        ParseLine();
}

toolkit::result<> scc::as::Parser::ParseLine()
{
    if (At(TokenType::Label))
    {
        auto label = Skip().Value;

        const auto is_local = label.front() == '.';

        if (is_local)
        {
            if (!m_Primary)
                return toolkit::make_error("local label '{}' in global space", label);

            label = m_Primary->GetName() + label;
        }

        auto *symbol = m_Module.GetOrCreateSymbol(label);
        symbol->SetLocation(*m_Section, m_Section->size());

        if (!is_local)
            m_Primary = symbol;
    }

    if (Skip(TokenType::EndOfLine))
        return {};

    if (Skip(TokenType::Symbol, ".section"))
    {
        std::string name;
        auto set_name = [&name](Token &&token) -> toolkit::result<>
        {
            name = std::move(token.Value);
            return {};
        };

        if (auto res = Expect(TokenType::Symbol) & set_name; !res)
            return res;

        m_Section = m_Module.GetOrCreateSection(name);

        if (auto res = Expect(TokenType::EndOfLine); !res)
            return res;

        return {};
    }

    if (At(TokenType::Symbol) && m_Token.Value.front() == '.')
    {
        const auto directive = Skip().Value;
        std::vector<OperandPtr> operands;

        while (!At(TokenType::EndOfLine))
        {
            OperandPtr operand;
            if (auto res = ParseDirectiveOperand() >> operand; !res)
                return res;

            operands.push_back(std::move(operand));

            if (!At(TokenType::EndOfLine))
                if (auto res = Expect(TokenType::Other, ","); !res)
                    return res;
        }

        (void) Evaluate(directive, operands);
    }
    else
    {
        InstructionPtr instruction;
        if (auto res = ParseInstruction() >> instruction; !res)
            return res;

        m_Section->Insert(std::move(instruction));
    }

    if (auto res = Expect(TokenType::EndOfLine); !res)
        return res;

    return {};
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseDirectiveOperand()
{
    if (Skip(TokenType::Other, "("))
    {
        std::string directive;
        auto set_directive = [&directive](Token &&token) -> toolkit::result<>
        {
            directive = std::move(token.Value);
            return {};
        };

        if (auto res = Expect(TokenType::Symbol) & set_directive; !res)
            return res;

        std::vector<OperandPtr> operands;

        if (directive.front() != '.')
            return toolkit::make_error("symbol '{}' is not a directive", directive);

        while (!At(TokenType::Other, ")"))
        {
            OperandPtr operand;
            if (auto res = ParseDirectiveOperand() >> operand; !res)
                return res;

            operands.push_back(std::move(operand));

            if (!At(TokenType::Other, ")"))
                if (auto res = Expect(TokenType::Other, ","); !res)
                    return res;
        }

        if (auto res = Expect(TokenType::Other, ")"); !res)
            return res;

        return Evaluate(directive, operands);
    }

    if (At(TokenType::Symbol))
    {
        const auto name = Skip().Value;

        return { std::make_unique<SymbolAddressOperand>(m_Platform, m_Module.GetOrCreateSymbol(name)) };
    }

    if (At(TokenType::Immediate))
    {
        auto immediate = Skip().Immediate;

        return { std::make_unique<ImmediateOperand>(m_Platform, immediate) };
    }

    if (At(TokenType::String))
    {
        auto string = Skip().Value;

        return { std::make_unique<StringOperand>(m_Platform, std::move(string)) };
    }

    return toolkit::make_error(
        "unhandled directive operand token type {}, raw value '{}'",
        m_Token.Type,
        m_Token.Raw);
}

toolkit::result<scc::as::InstructionPtr> scc::as::Parser::ParseInstruction()
{
    std::string name;
    auto set_name = [&name](Token &&token) -> toolkit::result<>
    {
        name = std::move(token.Value);
        return {};
    };

    if (auto res = Expect(TokenType::Symbol) & set_name; !res)
        return res;

    const auto mnemonic = m_Platform.ISA.FindMnemonicName(name);

    if (!mnemonic)
        return toolkit::make_error("undefined mnemonic '{}'", name);

    if (At(TokenType::EndOfLine))
        return std::make_unique<Instruction>(m_Platform, *mnemonic);

    std::vector<OperandPtr> operands;
    if (auto res = ParseOperands() >> operands; !res)
        return res;

    return std::make_unique<Instruction>(m_Platform, *mnemonic, std::move(operands));
}

toolkit::result<std::vector<scc::as::OperandPtr>> scc::as::Parser::ParseOperands()
{
    std::vector<OperandPtr> operands;

    do
    {
        OperandPtr operand;
        if (auto res = ParseOperand() >> operand; !res)
            return res;

        operands.push_back(std::move(operand));
    }
    while (Skip(TokenType::Other, ","));

    return operands;
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseOperand()
{
    if (Skip(TokenType::Other, "$"))
    {
        if (At(TokenType::Symbol))
        {
            auto label = Skip().Value;

            if (label.front() == '.')
            {
                if (!m_Primary)
                    return toolkit::make_error("local label '{}' in global space", label);

                label = m_Primary->GetName() + label;
            }

            return { std::make_unique<SymbolAddressOperand>(m_Platform, m_Module.GetOrCreateSymbol(label)) };
        }

        if (At(TokenType::Immediate))
        {
            auto value = Skip().Immediate;

            if (!At(TokenType::Other, "("))
                return { std::make_unique<ImmediateOperand>(m_Platform, value) };
        }

        return toolkit::make_error(
            "unhandled immediate operand token type {}, raw value '{}'",
            m_Token.Type,
            m_Token.Raw);
    }

    if (At(TokenType::Register))
    {
        const auto name = Skip().Value;

        if (const auto reg = m_Platform.ISA.FindRegisterName(name))
            return { std::make_unique<RegisterOperand>(m_Platform, *reg) };

        return toolkit::make_error("undefined register '{}'", name);
    }

    if (At(TokenType::Symbol))
    {
        auto label = Skip().Value;

        if (label.front() == '.')
        {
            if (!m_Primary)
                return toolkit::make_error("local label '{}' in global space", label);

            label = m_Primary->GetName() + label;
        }

        return { std::make_unique<SymbolOperand>(m_Platform, m_Module.GetOrCreateSymbol(label)) };
    }

    Immediate immediate{};
    if (At(TokenType::Immediate))
    {
        immediate = Skip().Immediate;

        if (!At(TokenType::Other, "("))
            return { std::make_unique<ReferenceOperand>(m_Platform, immediate) };
    }

    if (Skip(TokenType::Other, "("))
    {
        std::string base_name;
        auto set_base_name = [&base_name](Token &&token) -> toolkit::result<>
        {
            base_name = std::move(token.Value);
            return {};
        };

        if (auto res = Expect(TokenType::Register) & set_base_name; !res)
            return res;

        std::string index_name;
        auto set_index_name = [&index_name](Token &&token) -> toolkit::result<>
        {
            index_name = std::move(token.Value);
            return {};
        };

        Immediate scale = 0;
        auto set_scale = [&scale](Token &&token) -> toolkit::result<>
        {
            scale = token.Immediate;
            return {};
        };

        if (Skip(TokenType::Other, ","))
        {
            if (auto res = Expect(TokenType::Register) & set_index_name; !res)
                return res;

            if (Skip(TokenType::Other, ","))
                if (auto res = Expect(TokenType::Immediate) & set_scale; !res)
                    return res;
        }

        if (auto res = Expect(TokenType::Other, ")"); !res)
            return res;

        auto has_base_register = !base_name.empty();
        Register base_register{};

        if (has_base_register)
        {
            const auto reg = m_Platform.ISA.FindRegisterName(base_name);

            if (!reg)
                return toolkit::make_error("undefined register '{}'", base_name);

            base_register = *reg;
        }

        auto has_index_register = !index_name.empty();
        Register index_register{};

        if (has_index_register)
        {
            const auto reg = m_Platform.ISA.FindRegisterName(index_name);

            if (!reg)
                return toolkit::make_error("undefined register '{}'", index_name);

            index_register = *reg;
        }

        return {
            std::make_unique<ReferenceOperand>(
                m_Platform,
                immediate,
                has_base_register,
                base_register,
                has_index_register,
                index_register,
                scale)
        };
    }

    return toolkit::make_error(
        "unhandled operand token type {}, raw value '{}'",
        m_Token.Type,
        m_Token.Raw);
}

static toolkit::result<scc::as::OperandPtr> directive_set(
    const scc::as::EvaluationContext &,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".set requires 2 operands, got {}", operands.size());

    const auto *dst = dynamic_cast<scc::as::SymbolAddressOperand *>(operands[0].get());

    if (!dst)
        return toolkit::make_error("1st operand for .set must be symbol");

    auto *symbol = dst->GetSymbol();
    const auto value = operands[1]->GetImmediate();

    symbol->SetAddress(value);

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_add(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".add requires 2 operands, got {}", operands.size());

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, lhs + rhs) };
}

static toolkit::result<scc::as::OperandPtr> directive_sub(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".sub requires 2 operands, got {}", operands.size());

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, lhs - rhs) };
}

static toolkit::result<scc::as::OperandPtr> directive_length(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".length requires 1 operand, got {}", operands.size());

    const auto *src = dynamic_cast<scc::as::SymbolAddressOperand *>(operands[0].get());

    if (!src)
        return toolkit::make_error("1st operand for .length must be symbol");

    const auto *symbol = src->GetSymbol();
    const auto *fragment = dynamic_cast<scc::as::Data *>(symbol->GetFragment());

    auto value = fragment->GetDataSize();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, value) };
}

static toolkit::result<scc::as::OperandPtr> directive_fill(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".fill requires 2 operands, got {}", operands.size());

    auto count = operands[0]->GetImmediate();
    auto value = operands[1]->GetImmediate();

    context.Section->Insert(std::make_unique<scc::as::Fill>(count, value));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_align(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".align requires 1 operand, got {}", operands.size());

    auto alignment = operands[0]->GetImmediate();

    context.Section->Insert(std::make_unique<scc::as::Align>(alignment));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_string(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".string requires 1 operand, got {}", operands.size());

    const auto *operand = dynamic_cast<scc::as::StringOperand *>(operands[0].get());

    auto &value = operand->GetValue();

    std::vector<uint8_t> data(value.size() + 1);
    for (size_t i = 0; i < value.size(); ++i)
        data[i] = value[i];
    data[value.size()] = 0;

    context.Section->Insert(std::make_unique<scc::as::Data>(std::move(data)));

    return {};
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::Evaluate(
    const std::string &directive,
    const std::vector<OperandPtr> &operands) const
{
    static const std::unordered_map<std::string, toolkit::result<OperandPtr>(*)(
        const EvaluationContext &,
        const std::vector<OperandPtr> &)> map
    {
        { ".set", directive_set },
        { ".add", directive_add },
        { ".sub", directive_sub },
        { ".length", directive_length },
        { ".fill", directive_fill },
        { ".align", directive_align },
        { ".string", directive_string },
    };

    const auto it = map.find(directive);
    if (it == map.end())
        return toolkit::make_error("undefined directive '{}'", directive);

    return it->second(
        {
            .Platform = m_Platform,
            .Section = m_Section,
        },
        operands);
}

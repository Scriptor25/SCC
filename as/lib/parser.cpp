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

toolkit::result<> scc::as::Parser::Parse()
{
    while (m_Token.Type != TokenType::EndOfFile)
        if (auto res = ParseLine(); !res)
            return res;

    return {};
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
                return toolkit::make_error("invalid local label '{}'", label);

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
        if (auto res = ParseAndEvaluateDirective(TokenType::EndOfLine); !res)
            return res;
        return {};
    }

    InstructionPtr instruction;
    if (auto res = ParseInstruction() >> instruction; !res)
        return res;

    m_Section->Insert(std::move(instruction));

    if (auto res = Expect(TokenType::EndOfLine); !res)
        return res;

    return {};
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseAndEvaluateDirective(
    TokenType end_type,
    const std::string &end_value)
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

    while (!At(end_type, end_value))
    {
        OperandPtr operand;
        if (auto res = ParseDirectiveOperand() >> operand; !res)
            return res;

        operands.push_back(std::move(operand));

        if (!At(end_type, end_value))
            if (auto res = Expect(TokenType::Other, ","); !res)
                return res;
    }

    if (auto res = Expect(end_type, end_value); !res)
        return res;

    return Evaluate(directive, operands);
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseDirectiveOperand()
{
    if (Skip(TokenType::Other, "("))
        return ParseAndEvaluateDirective(TokenType::Other, ")");

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
        if (auto res = ParseOperand(true) >> operand; !res)
            return res;

        operands.push_back(std::move(operand));
    }
    while (Skip(TokenType::Other, ","));

    return operands;
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseOperand(bool selector)
{
    if (At(TokenType::Other, "$"))
        return ParseAddressOperand();

    if (At(TokenType::Symbol))
        return ParseSymbolOperand();

    if (At(TokenType::Register))
        return ParseRegisterOperand(selector);

    return ParseMemoryOperand(selector);
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseAddressOperand()
{
    if (auto res = Expect(TokenType::Other, "$"); !res)
        return res;

    if (At(TokenType::Symbol))
    {
        Symbol *symbol;
        if (auto res = ParseSymbol() >> symbol; !res)
            return res;

        return { std::make_unique<SymbolAddressOperand>(m_Platform, symbol) };
    }

    if (At(TokenType::Immediate))
    {
        auto value = Skip().Immediate;

        return { std::make_unique<ImmediateOperand>(m_Platform, value) };
    }

    return toolkit::make_error(
        "unhandled immediate operand token type {}, raw value '{}'",
        m_Token.Type,
        m_Token.Raw);
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseSymbolOperand()
{
    Symbol *symbol;
    if (auto res = ParseSymbol() >> symbol; !res)
        return res;

    return { std::make_unique<SymbolOperand>(m_Platform, symbol) };
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseRegisterOperand(bool selector)
{
    std::string name;
    auto set_name = [&name](Token &&token) -> toolkit::result<>
    {
        name = std::move(token.Value);
        return {};
    };

    if (auto res = Expect(TokenType::Register) & set_name; !res)
        return res;

    auto reg = m_Platform.ISA.FindRegisterName(name);
    if (!reg)
        return toolkit::make_error("undefined register '{}'", name);

    if (!selector || !Skip(TokenType::Other, ":"))
        return { std::make_unique<RegisterOperand>(m_Platform, *reg) };

    auto *view = m_Platform.ISA.FindRegisterView(*reg);
    if (!view)
        return toolkit::make_error("view must not be null");

    auto &desc = m_Platform.ISA.Registers.at(view->Code);
    if (desc.Class != platform::RegisterClass::Segment)
        return toolkit::make_error("expected segment register");

    OperandPtr operand;
    if (auto res = ParseOperand(false) >> operand; !res)
        return res;

    operand->SetSegmentRegister(*reg);

    return operand;
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::ParseMemoryOperand(bool selector)
{
    Immediate displacement{};
    if (At(TokenType::Immediate))
    {
        displacement = Skip().Immediate;

        if (selector && Skip(TokenType::Other, ":"))
        {
            if (displacement < 0x00 || displacement >= 0xFF)
                return toolkit::make_error("invalid segment selector '{}'", displacement);

            OperandPtr operand;
            if (auto res = ParseOperand(false) >> operand; !res)
                return res;

            operand->SetSegmentSelector(displacement);

            return operand;
        }

        if (!At(TokenType::Other, "("))
            return { std::make_unique<MemoryOperand>(m_Platform, displacement) };
    }

    if (auto res = Expect(TokenType::Other, "("); !res)
        return res;

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

    Immediate scale{};
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

    std::optional<Register> base_register;
    if (!base_name.empty())
    {
        const auto reg = m_Platform.ISA.FindRegisterName(base_name);

        if (!reg)
            return toolkit::make_error("undefined register '{}'", base_name);

        base_register = *reg;
    }

    std::optional<Register> index_register;
    if (!index_name.empty())
    {
        const auto reg = m_Platform.ISA.FindRegisterName(index_name);

        if (!reg)
            return toolkit::make_error("undefined register '{}'", index_name);

        index_register = *reg;
    }

    return { std::make_unique<MemoryOperand>(m_Platform, displacement, base_register, index_register, scale) };
}

toolkit::result<scc::as::Symbol *> scc::as::Parser::ParseSymbol()
{
    std::string label;
    auto set_label = [&label](Token &&token) -> toolkit::result<>
    {
        label = std::move(token.Value);
        return {};
    };

    if (auto res = Expect(TokenType::Symbol) & set_label; !res)
        return res;

    if (label.front() == '.')
    {
        if (!m_Primary)
            return toolkit::make_error("invalid local label '{}'", label);

        label = m_Primary->GetName() + label;
    }

    return m_Module.GetOrCreateSymbol(label);
}

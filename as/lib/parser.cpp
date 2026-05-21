#include <scc/as/data.hpp>
#include <scc/as/fill.hpp>
#include <scc/as/instruction.hpp>
#include <scc/as/module.hpp>
#include <scc/as/operand.hpp>
#include <scc/as/parser.hpp>

#include <scc/assert.hpp>
#include <scc/error.hpp>
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

scc::as::Parser::Parser(std::istream &stream, const Platform &platform, Module &module)
    : m_Stream(stream),
      m_Platform(platform),
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

scc::as::Token scc::as::Parser::Expect(const TokenType type, const std::string &value)
{
    if (m_Token.Type != type)
        Error("TODO");

    if (!value.empty() && m_Token.Value != value)
        Error("TODO");

    return Skip();
}

void scc::as::Parser::Parse()
{
    while (m_Token.Type != TokenType::EndOfFile)
        ParseLine();
}

void scc::as::Parser::ParseLine()
{
    if (At(TokenType::Label))
    {
        auto label = Skip().Value;

        const auto local = label.front() == '.';

        if (local)
        {
            Assert(!!m_Primary, "local label '{}' in global space", label);

            label = m_Primary->GetName() + label;
        }

        auto *symbol = m_Module.GetOrCreateSymbol(label);
        symbol->SetLocation(*m_Section, m_Section->GetFragmentCount());

        if (!local)
        {
            m_Primary = symbol;
        }
    }

    if (Skip(TokenType::EndOfLine))
        return;

    if (Skip(TokenType::Symbol, ".section"))
    {
        const auto name = Expect(TokenType::Symbol).Value;

        m_Section = m_Module.GetOrCreateSection(name);

        Expect(TokenType::EndOfLine);
        return;
    }

    if (At(TokenType::Symbol) && m_Token.Value.front() == '.')
    {
        const auto directive = Skip().Value;
        std::vector<OperandPtr> operands;

        while (!At(TokenType::EndOfLine))
        {
            operands.push_back(ParseDirectiveOperand());

            if (!At(TokenType::EndOfLine))
                Expect(TokenType::Other, ",");
        }

        Evaluate(directive, operands);
    }
    else
    {
        m_Section->Insert(ParseInstruction());
    }

    Expect(TokenType::EndOfLine);
}

scc::as::OperandPtr scc::as::Parser::ParseDirectiveOperand()
{
    if (Skip(TokenType::Other, "("))
    {
        const auto directive = Expect(TokenType::Symbol).Value;
        std::vector<OperandPtr> operands;

        Assert(directive.front() == '.', "TODO");

        while (!At(TokenType::Other, ")"))
        {
            operands.push_back(ParseDirectiveOperand());

            if (!At(TokenType::Other, ")"))
                Expect(TokenType::Other, ",");
        }

        Expect(TokenType::Other, ")");

        return Evaluate(directive, operands);
    }

    if (At(TokenType::Symbol))
    {
        const auto name = Skip().Value;

        return std::make_unique<SymbolAddressOperand>(m_Platform, m_Module.GetOrCreateSymbol(name));
    }

    if (At(TokenType::Immediate))
    {
        auto immediate = Skip().Immediate;

        return std::make_unique<ImmediateOperand>(m_Platform, immediate);
    }

    if (At(TokenType::String))
    {
        auto string = Skip().Value;

        return std::make_unique<StringOperand>(m_Platform, std::move(string));
    }

    Error("TODO");
}

scc::as::Instruction scc::as::Parser::ParseInstruction()
{
    const auto name = Expect(TokenType::Symbol).Value;

    const auto it = m_Platform.ISA.MnemonicNames.find(name);
    Assert(it != m_Platform.ISA.MnemonicNames.end(), "undefined mnemonic '{}'", name);

    if (At(TokenType::EndOfLine))
        return Instruction(m_Platform, it->second);

    auto operands = ParseOperands();

    return Instruction(m_Platform, it->second, std::move(operands));
}

std::vector<scc::as::OperandPtr> scc::as::Parser::ParseOperands()
{
    std::vector<OperandPtr> operands;

    do
        operands.push_back(ParseOperand());
    while (Skip(TokenType::Other, ","));

    return operands;
}

scc::as::OperandPtr scc::as::Parser::ParseOperand()
{
    if (Skip(TokenType::Other, "$"))
    {
        if (At(TokenType::Symbol))
        {
            auto label = Skip().Value;

            if (label.front() == '.')
            {
                Assert(!!m_Primary, "local label '{}' in global space", label);

                label = m_Primary->GetName() + label;
            }

            return std::make_unique<SymbolAddressOperand>(m_Platform, m_Module.GetOrCreateSymbol(label));
        }

        if (At(TokenType::Immediate))
        {
            auto value = Skip().Immediate;

            if (!At(TokenType::Other, "("))
                return std::make_unique<ImmediateOperand>(m_Platform, value);
        }

        Error("TODO");
    }

    if (At(TokenType::Register))
    {
        const auto name = Skip().Value;

        const auto it = m_Platform.ISA.RegisterNames.find(name);
        Assert(it != m_Platform.ISA.RegisterNames.end(), "undefined register '{}'", name);

        return std::make_unique<RegisterOperand>(m_Platform, it->second);
    }

    if (At(TokenType::Symbol))
    {
        auto label = Skip().Value;

        if (label.front() == '.')
        {
            Assert(!!m_Primary, "local label '{}' in global space", label);

            label = m_Primary->GetName() + label;
        }

        return std::make_unique<SymbolOperand>(m_Platform, m_Module.GetOrCreateSymbol(label));
    }

    Immediate immediate{};
    if (At(TokenType::Immediate))
    {
        immediate = Skip().Immediate;

        if (!At(TokenType::Other, "("))
            return std::make_unique<DirectOperand>(m_Platform, immediate);
    }

    if (Skip(TokenType::Other, "("))
    {
        const auto base_name = Expect(TokenType::Register).Value;

        std::string index_name;
        Immediate scale = 0;

        if (Skip(TokenType::Other, ","))
        {
            index_name = Expect(TokenType::Register).Value;

            if (Skip(TokenType::Other, ","))
            {
                scale = Expect(TokenType::Immediate).Immediate;
            }
        }

        Expect(TokenType::Other, ")");

        Register base_register{};
        if (!base_name.empty())
        {
            const auto it = m_Platform.ISA.RegisterNames.find(base_name);
            Assert(it != m_Platform.ISA.RegisterNames.end(), "undefined register '{}'", base_name);

            base_register = it->second;
        }

        Register index_register{};
        if (!index_name.empty())
        {
            const auto it = m_Platform.ISA.RegisterNames.find(index_name);
            Assert(it != m_Platform.ISA.RegisterNames.end(), "undefined register '{}'", index_name);

            index_register = it->second;
        }

        return std::make_unique<ReferenceOperand>(
            m_Platform,
            immediate,
            base_register,
            index_register,
            scale);
    }

    Error("TODO");
}

static scc::as::OperandPtr directive_set(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    scc::Assert(operands.size() == 2, ".set requires 2 operands");

    const auto *dst = dynamic_cast<scc::as::SymbolAddressOperand *>(operands[0].get());
    scc::Assert(!!dst, "1st operand for .set must be symbol");

    auto *symbol = dst->GetSymbol();
    const auto value = operands[1]->GetImmediate();

    symbol->SetAddress(value);

    return {};
}

static scc::as::OperandPtr directive_add(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    scc::Assert(operands.size() == 2, ".add requires 2 operands");

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return std::make_unique<scc::as::ImmediateOperand>(context.MPlatform, lhs + rhs);
}

static scc::as::OperandPtr directive_sub(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    scc::Assert(operands.size() == 2, ".sub requires 2 operands");

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return std::make_unique<scc::as::ImmediateOperand>(context.MPlatform, lhs - rhs);
}

static scc::as::OperandPtr directive_length(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    scc::Assert(operands.size() == 1, ".length requires 1 operand");

    const auto *src = dynamic_cast<scc::as::SymbolAddressOperand *>(operands[0].get());

    scc::Assert(!!src, "1st operand for .length must be symbol");

    const auto *symbol = src->GetSymbol();
    const auto *fragment = dynamic_cast<scc::as::Data *>(symbol->GetFragment());

    auto value = fragment->GetDataSize();

    return std::make_unique<scc::as::ImmediateOperand>(context.MPlatform, value);
}

static scc::as::OperandPtr directive_fill(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    scc::Assert(operands.size() == 2, ".fill requires 2 operands");

    auto count = operands[0]->GetImmediate();
    auto value = operands[1]->GetImmediate();

    context.MSection->Insert(std::make_unique<scc::as::Fill>(count, value));

    return {};
}

static scc::as::OperandPtr directive_string(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    scc::Assert(operands.size() == 1, ".string requires 1 operands");

    const auto *operand = dynamic_cast<scc::as::StringOperand *>(operands[0].get());

    auto &value = operand->GetValue();

    std::vector<uint8_t> data(value.size() + 1);
    for (size_t i = 0; i < value.size(); ++i)
        data[i] = value[i];
    data[value.size()] = 0;

    context.MSection->Insert(std::make_unique<scc::as::Data>(std::move(data)));

    return {};
}

scc::as::OperandPtr scc::as::Parser::Evaluate(
    const std::string &directive,
    const std::vector<OperandPtr> &operands)
{
    static const std::unordered_map<std::string, OperandPtr(*)(
        const EvaluationContext &,
        const std::vector<OperandPtr> &)> map
    {
        { ".set", directive_set },
        { ".add", directive_add },
        { ".sub", directive_sub },
        { ".length", directive_length },
        { ".fill", directive_fill },
        { ".string", directive_string },
    };

    const auto it = map.find(directive);
    Assert(it != map.end(), "undefined directive '{}'", directive);

    return it->second(
        {
            .MPlatform = m_Platform,
            .MSection = m_Section,
        },
        operands);
}

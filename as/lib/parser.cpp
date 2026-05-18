#include <scc/as/instruction.hpp>
#include <scc/as/module.hpp>
#include <scc/as/operand.hpp>
#include <scc/as/parser.hpp>

#include <scc/platform.hpp>

#include <istream>
#include <memory>
#include <vector>
#include <scc/assert.hpp>

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
      m_Symbol()
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
                                           ? static_cast<Immediate>(std::stoll(value, nullptr, base))
                                           : std::stoull(value, nullptr, base);
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

scc::as::Token scc::as::Parser::Expect(const TokenType type, const std::string &value)
{
    if (m_Token.Type != type)
        throw std::runtime_error("TODO");

    if (!value.empty() && m_Token.Value != value)
        throw std::runtime_error("TODO");

    return Skip();
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

void scc::as::Parser::Parse()
{
    while (m_Token.Type != TokenType::EndOfFile)
        ParseLine();
}

void scc::as::Parser::ParseLine()
{
    if (At(TokenType::Label))
    {
        const auto label = Skip().Value;
        m_Symbol = m_Module.GetOrCreateSymbol(label);
    }

    if (At(TokenType::EndOfLine))
    {
        Next();
        return;
    }

    if (At(TokenType::Symbol) && m_Token.Value.front() == '.')
    {
        auto directive = Skip().Value;

        while (!At(TokenType::EndOfLine))
        {
            ParseDirectiveArg();

            if (!At(TokenType::EndOfLine))
                Expect(TokenType::Other, ",");
        }

        if (m_Symbol)
        {
            // TODO: set symbol fragment
            m_Symbol = {};
        }
    }
    else
    {
        auto *fragment = m_Section->Insert(ParseInstruction());

        if (m_Symbol)
        {
            m_Symbol->SetFragment(*fragment);
            m_Symbol = {};
        }
    }

    Expect(TokenType::EndOfLine);
}

void scc::as::Parser::ParseDirectiveArg()
{
    if (Skip(TokenType::Other, "("))
    {
        auto directive = Expect(TokenType::Symbol).Value;
        // TODO: use directive

        if (directive.front() != '.')
            throw std::runtime_error("TODO");

        while (!At(TokenType::Other, ")"))
        {
            ParseDirectiveArg();
            // TODO: use directive arg

            if (!At(TokenType::Other, ")"))
                Expect(TokenType::Other, ",");
        }

        Expect(TokenType::Other, ")");

        return;
    }

    if (At(TokenType::Symbol))
    {
        auto symbol = Skip();
        // TODO: use symbol

        return;
    }

    if (At(TokenType::Immediate))
    {
        auto immediate = Skip();
        // TODO: use immediate

        return;
    }

    if (At(TokenType::String))
    {
        auto string = Skip();
        // TODO: use string

        return;
    }

    throw std::runtime_error("TODO");
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

std::vector<std::unique_ptr<scc::as::Operand>> scc::as::Parser::ParseOperands()
{
    std::vector<std::unique_ptr<Operand>> operands;

    do
        operands.push_back(ParseOperand());
    while (Skip(TokenType::Other, ","));

    return operands;
}

std::unique_ptr<scc::as::Operand> scc::as::Parser::ParseOperand()
{
    if (At(TokenType::Symbol))
    {
        const auto name = Skip().Value;

        return std::make_unique<SymbolOperand>(m_Platform, m_Module.GetOrCreateSymbol(name));
    }

    if (At(TokenType::Register))
    {
        const auto name = Skip().Value;

        const auto it = m_Platform.ISA.RegisterNames.find(name);
        Assert(it != m_Platform.ISA.RegisterNames.end(), "undefined register '{}'", name);

        return std::make_unique<RegisterOperand>(m_Platform, it->second);
    }

    Immediate immediate{};
    bool has_immediate{};

    if (At(TokenType::Immediate))
    {
        immediate = Skip().Immediate;
        has_immediate = true;
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

    if (has_immediate)
        return std::make_unique<ImmediateOperand>(m_Platform, immediate);

    throw std::runtime_error("TODO");
}

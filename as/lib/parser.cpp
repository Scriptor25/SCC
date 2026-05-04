#include <scc/as/parser.hpp>

#include <istream>

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

scc::as::Parser::Parser(std::istream &stream)
    : m_Stream(stream)
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
        throw std::runtime_error("");

    if (!value.empty() && m_Token.Value != value)
        throw std::runtime_error("");

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

bool scc::as::Parser::SkipIf(const TokenType type, const std::string &value)
{
    if (!At(type, value))
        return false;

    Skip();
    return true;
}

void scc::as::Parser::Parse()
{
    while (m_Token.Type != TokenType::EndOfFile)
    {
        ParseLine();
        // TODO: use line
    }
}

void scc::as::Parser::ParseLine()
{
    if (At(TokenType::Label))
    {
        auto label = Skip();
        // TODO: use label
    }

    if (At(TokenType::EndOfLine))
    {
        Next();
        return;
    }

    if (At(TokenType::Symbol) && m_Token.Value.front() == '.')
    {
        auto directive = Skip();
        // TODO: use directive

        while (!At(TokenType::EndOfLine))
        {
            ParseDirectiveArg();
            // TODO: use directive arg
        }
    }
    else
    {
        ParseInstruction();
        // TODO: use instruction
    }

    Expect(TokenType::EndOfLine);
}

void scc::as::Parser::ParseDirectiveArg()
{
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

    throw std::runtime_error("");
}

void scc::as::Parser::ParseInstruction()
{
    auto mnemonic = Expect(TokenType::Symbol);
    // TODO: use mnemonic

    if (At(TokenType::EndOfLine))
        return;

    ParseOperandList();
    // TODO: use operand list
}

void scc::as::Parser::ParseOperandList()
{
    do
    {
        ParseOperand();
        // TODO: use operand
    }
    while (SkipIf(TokenType::Other, ","));
}

void scc::as::Parser::ParseOperand()
{
    if (SkipIf(TokenType::Other, "$"))
    {
        if (At(TokenType::Symbol))
        {
            auto symbol = Skip();
            // TODO: use symbol

            return;
        }

        auto immediate = Expect(TokenType::Immediate);
        // TODO: use immediate

        return;
    }

    if (At(TokenType::Register))
    {
        Skip();
        return;
    }

    if (At(TokenType::Immediate))
    {
        auto displacement = Skip();
        // TODO: use displacement
    }

    Expect(TokenType::Other, "(");

    auto base_register = Expect(TokenType::Register);
    // TODO: use base register

    if (SkipIf(TokenType::Other, ","))
    {
        auto index_register = Expect(TokenType::Register);
        // TODO: use index register

        if (SkipIf(TokenType::Other, ","))
        {
            auto scale = Expect(TokenType::Immediate);
            // TODO: use scale
        }
    }

    Expect(TokenType::Other, ")");
}

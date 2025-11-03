#include <istream>
#include <scc/as/parser.hpp>

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
    m_Buf = stream.get();
    m_Token = Get();
}

scc::as::Token scc::as::Parser::Get()
{
    enum StateE
    {
        State_None,
        State_Comment,
        State_Immediate,
        State_Character,
        State_String,
        State_Symbol,
    };

    std::string raw;
    std::string value;

    auto state = State_None;
    auto base = 0;
    auto reg = false;

    while (m_Buf >= 0)
    {
        switch (state)
        {
        case State_None:
            switch (m_Buf)
            {
            case '\n':
                raw += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                return { .Type = TokenType_EOL, .Raw = std::move(raw) };

            case '#':
                state = State_Comment;
                break;

            case '0':
                raw += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                if (m_Buf == 'x')
                {
                    raw += static_cast<char>(m_Buf);
                    m_Buf = m_Stream.get();
                    base = 16;
                }
                else
                {
                    value += '0';
                    base = 8;
                }
                state = State_Immediate;
                break;

            case '%':
                raw += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                reg = true;
                state = State_Symbol;
                break;

            case '\'':
                raw += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                state = State_Character;
                break;

            case '"':
                raw += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                state = State_String;
                break;

            default:
                if (std::isspace(m_Buf))
                {
                    raw += static_cast<char>(m_Buf);
                    m_Buf = m_Stream.get();
                    break;
                }

                if (std::isdigit(m_Buf))
                {
                    base = 10;
                    state = State_Immediate;
                    break;
                }

                if (std::isalpha(m_Buf) || m_Buf == '_' || m_Buf == '.')
                {
                    state = State_Symbol;
                    break;
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                return { .Type = TokenType_Other, .Raw = std::move(raw), .Value = std::move(value) };
            }
            break;

        case State_Comment:
            if (m_Buf == '\n')
            {
                state = State_None;
                break;
            }
            raw += static_cast<char>(m_Buf);
            m_Buf = m_Stream.get();
            break;

        case State_Immediate:
            if (!isdigit(m_Buf, base))
            {
                const auto immediate = std::stoll(value, nullptr, base);
                return {
                    .Type = TokenType_Immediate,
                    .Raw = std::move(raw),
                    .Value = std::move(value),
                    .Immediate = immediate,
                };
            }

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            m_Buf = m_Stream.get();
            break;

        case State_Character:
            if (m_Buf == '\'')
            {
                raw += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                const auto immediate = value.front();
                return {
                    .Type = TokenType_Immediate,
                    .Raw = std::move(raw),
                    .Value = std::move(value),
                    .Immediate = immediate,
                };
            }

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            m_Buf = m_Stream.get();
            break;

        case State_String:
            if (m_Buf == '"')
            {
                raw += static_cast<char>(m_Buf);
                m_Buf = m_Stream.get();
                return {
                    .Type = TokenType_String,
                    .Raw = std::move(raw),
                    .Value = std::move(value),
                };
            }

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            m_Buf = m_Stream.get();
            break;

        case State_Symbol:
            if (!std::isalnum(m_Buf) && m_Buf != '_' && m_Buf != '.')
            {
                if (reg)
                {
                    return { .Type = TokenType_Register, .Raw = std::move(raw), .Value = std::move(value) };
                }

                if (m_Buf == ':')
                {
                    raw += static_cast<char>(m_Buf);
                    m_Buf = m_Stream.get();
                    return { .Type = TokenType_Label, .Raw = std::move(raw), .Value = std::move(value) };
                }

                return { .Type = TokenType_Symbol, .Raw = std::move(raw), .Value = std::move(value) };
            }

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            m_Buf = m_Stream.get();
            break;
        }
    }

    return { .Type = TokenType_EOF };
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

scc::as::Token scc::as::Parser::Expect(const TokenTypeE type, const std::string &value)
{
    if (m_Token.Type != type)
    {
        throw std::runtime_error("");
    }

    if (!value.empty() && m_Token.Value != value)
    {
        throw std::runtime_error("");
    }

    return Skip();
}

bool scc::as::Parser::At(const TokenTypeE type, const std::string &value) const
{
    if (m_Token.Type != type)
    {
        return false;
    }

    if (!value.empty() && m_Token.Value != value)
    {
        return false;
    }

    return true;
}

bool scc::as::Parser::SkipIf(const TokenTypeE type, const std::string &value)
{
    if (!At(type, value))
    {
        return false;
    }

    Skip();
    return true;
}

void scc::as::Parser::Parse()
{
    while (m_Token.Type)
    {
        ParseLine();
    }
}

void scc::as::Parser::ParseLine()
{
    if (At(TokenType_Label))
    {
        auto label = Skip();
    }

    if (At(TokenType_EOL))
    {
        Next();
        return;
    }

    if (At(TokenType_Symbol) && m_Token.Value.front() == '.')
    {
        auto directive = Skip();
        while (!At(TokenType_EOL))
        {
            ParseDirectiveArg();
        }
    }
    else
    {
        ParseInstruction();
    }

    Expect(TokenType_EOL);
}

void scc::as::Parser::ParseDirectiveArg()
{
    if (At(TokenType_Symbol))
    {
        auto symbol = Skip();
        return;
    }
    if (At(TokenType_Immediate))
    {
        auto immediate = Skip();
        return;
    }
    if (At(TokenType_String))
    {
        auto string = Skip();
        return;
    }
    throw std::runtime_error("");
}

void scc::as::Parser::ParseInstruction()
{
    auto mnemonic = Expect(TokenType_Symbol);

    if (At(TokenType_EOL))
    {
        return;
    }

    ParseOperandList();
}

void scc::as::Parser::ParseOperandList()
{
    do
    {
        ParseOperand();
    }
    while (SkipIf(TokenType_Other, ","));
}

void scc::as::Parser::ParseOperand()
{
    if (SkipIf(TokenType_Other, "$"))
    {
        if (At(TokenType_Symbol))
        {
            auto symbol = Skip();
            return;
        }

        auto immediate = Expect(TokenType_Immediate);
        return;
    }

    if (At(TokenType_Register))
    {
        Skip();
        return;
    }

    if (At(TokenType_Immediate))
    {
        auto displacement = Skip();
    }

    Expect(TokenType_Other, "(");

    auto base_register = Expect(TokenType_Register);

    if (SkipIf(TokenType_Other, ","))
    {
        auto index_register = Expect(TokenType_Register);

        if (SkipIf(TokenType_Other, ","))
        {
            auto scale = Expect(TokenType_Immediate);
        }
    }

    Expect(TokenType_Other, ")");
}

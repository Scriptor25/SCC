#include <scc/assert.hpp>
#include <scc/error.hpp>
#include <scc/ir/parser.hpp>

scc::ir::Parser::Parser(std::istream &stream)
    : m_Stream(stream)
{
    m_Buffer = m_Stream.get();
    Next();
}

int scc::ir::Parser::Get()
{
    return m_Buffer = m_Stream.get();
}

static bool isdigit(const unsigned base, const int c)
{
    switch (base)
    {
    case 0b10:
        return '0' <= c && c <= '1';
    case 010:
        return '0' <= c && c <= '7';
    case 10:
        return '0' <= c && c <= '9';
    case 0x10:
        return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
    default:
        return false;
    }
}

scc::ir::Token &scc::ir::Parser::Next()
{
    enum class State
    {
        None,
        Integer,
        Identifier,
        String,
    };

    std::string value;

    auto state = State::None;
    auto base = 0u;

    while (m_Buffer >= 0)
    {
        switch (state)
        {
        case State::None:
            if (m_Buffer == '0')
            {
                Get();
                switch (m_Buffer)
                {
                case 'b':
                    Get();
                    state = State::Integer;
                    base = 0b10u;
                    break;
                case 'x':
                    Get();
                    state = State::Integer;
                    base = 0x10u;
                    break;
                default:
                    value += '0';
                    state = State::Integer;
                    base = 010u;
                    break;
                }
                break;
            }
            if (m_Buffer == '"')
            {
                Get();
                state = State::String;
                break;
            }
            if (std::isdigit(m_Buffer))
            {
                state = State::Integer;
                base = 10u;
                break;
            }
            if (std::isalnum(m_Buffer))
            {
                state = State::Identifier;
                break;
            }
            value += static_cast<char>(m_Buffer);
            Get();
            return m_Token = {
                       .Type = TokenType::Other,
                       .Value = value,
                   };

        case State::Integer:
            if (isdigit(base, m_Buffer))
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            return m_Token = {
                       .Type = TokenType::Integer,
                       .Value = value,
                       .Integer = std::stoull(value, nullptr, static_cast<int>(base)),
                   };

        case State::Identifier:
            if (std::isalnum(m_Buffer))
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            return m_Token = {
                       .Type = TokenType::Identifier,
                       .Value = value,
                   };

        case State::String:
            if (m_Buffer != '"')
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            Get();
            return m_Token = {
                       .Type = TokenType::String,
                       .Value = value,
                   };
        }
    }

    return m_Token = {
               .Type = TokenType::EoF,
               .Value = {},
           };
}

bool scc::ir::Parser::Skip(const TokenType type, const std::string &value)
{
    if (m_Token.Type == type && (value.empty() || m_Token.Value == value))
    {
        Next();
        return true;
    }
    return false;
}

scc::ir::Token scc::ir::Parser::Skip()
{
    auto token = m_Token;
    Next();
    return token;
}

scc::ir::Token scc::ir::Parser::Expect(const TokenType type, const std::string &value)
{
    Assert(m_Token.Type == type && (value.empty() || m_Token.Value == value), "unexpected token");
    return Skip();
}

scc::ir::Token scc::ir::Parser::Expect(const std::vector<TokenType> &types)
{
    for (const auto &type : types)
    {
        if (m_Token.Type == type)
        {
            return Skip();
        }
    }
    Error("unexpected token");
}

scc::ir::Module scc::ir::Parser::ParseModule()
{
    Module module;

    while (m_Token.Type != TokenType::EoF)
    {
        if (Skip(TokenType::Other, "#"))
        {
            const auto key = Expect(TokenType::Identifier);
            if (key.Value == "name")
            {
                auto value = Expect(TokenType::String);
                module.SetName(std::move(value.Value));
            }

            continue;
        }

        Next();
    }

    return module;
}

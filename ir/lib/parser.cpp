#include <scc/assert.hpp>
#include <scc/error.hpp>
#include <scc/ir/parser.hpp>

scc::ir::Parser::Parser(std::istream &stream)
    : m_Stream(stream)
{
    Get();
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
    enum
    {
        state_none,
        state_integer,
        state_identifier,
        state_string,
    } state = state_none;

    std::string value;
    auto base = 0u;

    while (m_Buffer >= 0)
    {
        switch (state)
        {
        case state_none:
            if (m_Buffer == '0')
            {
                Get();
                switch (m_Buffer)
                {
                case 'b':
                    Get();
                    state = state_integer;
                    base = 0b10u;
                    break;
                case 'x':
                    Get();
                    state = state_integer;
                    base = 0x10u;
                    break;
                default:
                    value += '0';
                    state = state_integer;
                    base = 010u;
                    break;
                }
                break;
            }
            if (m_Buffer == '"')
            {
                Get();
                state = state_string;
                break;
            }
            if (std::isdigit(m_Buffer))
            {
                state = state_integer;
                base = 10u;
                break;
            }
            if (std::isalnum(m_Buffer))
            {
                state = state_identifier;
                break;
            }
            value += static_cast<char>(m_Buffer);
            Get();
            return m_Token = {
                       .Type = TokenType_Other,
                       .Value = value,
                   };

        case state_integer:
            if (isdigit(base, m_Buffer))
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            return m_Token = {
                       .Type = TokenType_Integer,
                       .Value = value,
                       .Integer = std::stoull(value, nullptr, base),
                   };

        case state_identifier:
            if (std::isalnum(m_Buffer))
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            return m_Token = {
                       .Type = TokenType_Identifier,
                       .Value = value,
                   };

        case state_string:
            if (m_Buffer != '"')
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            Get();
            return m_Token = {
                       .Type = TokenType_String,
                       .Value = value,
                   };
        }
    }

    return m_Token = {
               .Type = TokenType_EndOfFile,
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

    while (m_Token.Type)
    {
        if (Skip(TokenType_Other, "#"))
        {
            const auto key = Expect(TokenType_Identifier);
            if (key.Value == "name")
            {
                auto value = Expect(TokenType_String);
                module.SetName(std::move(value.Value));
            }

            continue;
        }

        Next();
    }

    return module;
}

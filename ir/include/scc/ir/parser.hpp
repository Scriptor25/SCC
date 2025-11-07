#pragma once

#include <iostream>
#include <scc/ir/module.hpp>

namespace scc::ir
{
    enum TokenType
    {
        TokenType_EndOfFile,
        TokenType_Identifier,
        TokenType_Integer,
        TokenType_String,
        TokenType_Other,
    };

    struct Token final
    {
        TokenType Type;
        std::string Value;
        uint64_t Integer = 0ull;
    };

    class Parser final
    {
    public:
        explicit Parser(std::istream &stream);

        int Get();
        Token &Next();

        bool Skip(TokenType type, const std::string &value = {});
        Token Skip();
        Token Expect(TokenType type, const std::string &value = {});
        Token Expect(const std::vector<TokenType> &types);

        template<typename... Types>
        Token Expect(Types &&... types)
        {
            return Expect(std::vector<TokenType>{ types... });
        }

        Module ParseModule();

    private:
        std::istream &m_Stream;
        int m_Buffer;
        Token m_Token;
    };
}

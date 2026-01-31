#pragma once

#include <iosfwd>
#include <scc/as/as.hpp>

namespace scc::as
{
    enum class TokenType
    {
        EoF,
        EoL,
        Label,
        Symbol,
        String,
        Register,
        Immediate,
        Other,
    };

    struct Token
    {
        TokenType Type = TokenType::EoF;
        std::string Raw;
        std::string Value;
        IImmediateT Immediate = 0;
    };

    class Parser
    {
    public:
        explicit Parser(std::istream &stream);

        Token Get();

        Token &Next();
        Token Skip();

        Token Expect(TokenType type, const std::string &value = {});
        [[nodiscard]] bool At(TokenType type, const std::string &value = {}) const;
        bool SkipIf(TokenType type, const std::string &value = {});

        void Parse();
        void ParseLine();
        void ParseDirectiveArg();
        void ParseInstruction();
        void ParseOperandList();
        void ParseOperand();

    private:
        std::istream &m_Stream;

        int m_Buffer;
        Token m_Token;
    };
}

#pragma once

#include <iosfwd>
#include <scc/as/as.hpp>

namespace scc::as
{
    enum TokenTypeE
    {
        TokenType_EOF,
        TokenType_EOL,
        TokenType_Label,
        TokenType_Symbol,
        TokenType_String,
        TokenType_Register,
        TokenType_Immediate,
        TokenType_Other,
    };

    struct Token
    {
        TokenTypeE Type = TokenType_EOF;
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

        Token Expect(TokenTypeE type, const std::string &value = {});
        [[nodiscard]] bool At(TokenTypeE type, const std::string &value = {}) const;
        bool SkipIf(TokenTypeE type, const std::string &value = {});

        void Parse();
        void ParseLine();
        void ParseDirectiveArg();
        void ParseInstruction();
        void ParseOperandList();
        void ParseOperand();

    private:
        std::istream &m_Stream;

        int m_Buf;
        Token m_Token;
    };
}

#pragma once

#include <scc/as/as.hpp>

#include <scc/common.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace scc::as
{
    enum class TokenType
    {
        EndOfFile,
        EndOfLine,
        Label,
        Symbol,
        String,
        Register,
        Immediate,
        Other,
    };

    struct Token
    {
        TokenType Type = TokenType::EndOfFile;
        std::string Raw;
        std::string Value;
        Immediate Immediate = 0;
    };

    class Parser
    {
    public:
        explicit Parser(std::istream &stream, const Platform &platform, Module &module);

        Token Get();

        Token &Next();
        Token Skip();

        Token Expect(TokenType type, const std::string &value = {});
        [[nodiscard]] bool At(TokenType type, const std::string &value = {}) const;
        bool Skip(TokenType type, const std::string &value = {});

        void Parse();
        void ParseLine();
        void ParseDirectiveArg();
        Instruction ParseInstruction();
        std::vector<std::unique_ptr<Operand>> ParseOperands();
        std::unique_ptr<Operand> ParseOperand();

    private:
        std::istream &m_Stream;

        int m_Buffer;
        Token m_Token;

        const Platform &m_Platform;

        Module &m_Module;
        Section *m_Section;
        Symbol *m_Symbol;
    };
}

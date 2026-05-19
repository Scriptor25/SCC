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

    struct EvaluationContext
    {
        const Platform &MPlatform;
        Section *MSection;
    };

    class Parser
    {
    public:
        explicit Parser(std::istream &stream, const Platform &platform, Module &module);

        Token Get();

        Token &Next();
        Token Skip();

        [[nodiscard]] bool At(TokenType type, const std::string &value = {}) const;

        bool Skip(TokenType type, const std::string &value = {});
        Token Expect(TokenType type, const std::string &value = {});

        void Parse();
        void ParseLine();

        OperandPtr ParseDirectiveOperand();

        Instruction ParseInstruction();
        std::vector<OperandPtr> ParseOperands();
        OperandPtr ParseOperand();

        OperandPtr Evaluate(const std::string &directive, const std::vector<OperandPtr> &operands);

    private:
        std::istream &m_Stream;

        int m_Buffer;
        Token m_Token;

        const Platform &m_Platform;

        Module &m_Module;
        Section *m_Section;
        Symbol *m_Primary;
    };
}

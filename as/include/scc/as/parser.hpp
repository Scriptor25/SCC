#pragma once

#include <scc/as/as.hpp>

#include <scc/common.hpp>

#include <toolkit/result.hxx>

#include <format>
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
        const Platform &Platform;
        Section *Section;
    };

    class Parser
    {
    public:
        explicit Parser(std::istream &stream, Module &module);

        Token Get();

        Token &Next();
        Token Skip();

        [[nodiscard]] bool At(TokenType type, const std::string &value = {}) const;

        bool Skip(TokenType type, const std::string &value = {});
        [[nodiscard]] toolkit::result<Token> Expect(TokenType type, const std::string &value = {});

        [[nodiscard]] toolkit::result<> Parse();
        [[nodiscard]] toolkit::result<> ParseLine();

        [[nodiscard]] toolkit::result<OperandPtr> ParseAndEvaluateDirective(
            TokenType end_type,
            const std::string &end_value = {});
        [[nodiscard]] toolkit::result<OperandPtr> ParseDirectiveOperand();

        [[nodiscard]] toolkit::result<InstructionPtr> ParseInstruction();
        [[nodiscard]] toolkit::result<std::vector<OperandPtr>> ParseOperands();

        [[nodiscard]] toolkit::result<OperandPtr> ParseOperand(bool selector);
        [[nodiscard]] toolkit::result<OperandPtr> ParseAddressOperand();
        [[nodiscard]] toolkit::result<OperandPtr> ParseSymbolOperand();
        [[nodiscard]] toolkit::result<OperandPtr> ParseRegisterOperand(bool selector);
        [[nodiscard]] toolkit::result<OperandPtr> ParseMemoryOperand(bool selector);

        [[nodiscard]] toolkit::result<Symbol *> ParseSymbol();

        [[nodiscard]] toolkit::result<OperandPtr> Evaluate(
            const std::string &directive,
            const std::vector<OperandPtr> &operands) const;

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

template<>
struct std::formatter<scc::as::TokenType>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    auto format(const scc::as::TokenType value, C &&ctx) const
    {
        static const std::unordered_map<scc::as::TokenType, const char *> map
        {
            { scc::as::TokenType::EndOfFile, "EndOfFile" },
            { scc::as::TokenType::EndOfLine, "EndOfLine" },
            { scc::as::TokenType::Label, "Label" },
            { scc::as::TokenType::Symbol, "Symbol" },
            { scc::as::TokenType::String, "String" },
            { scc::as::TokenType::Register, "Register" },
            { scc::as::TokenType::Immediate, "Immediate" },
            { scc::as::TokenType::Other, "Other" },
        };

        if (const auto it = map.find(value); it != map.end())
            return std::format_to(ctx.out(), "{}", it->second);

        return ctx.out();
    }
};

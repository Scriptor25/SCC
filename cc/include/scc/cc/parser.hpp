#pragma once

#include <scc/cc/node.hpp>

#include <toolkit/result.hxx>

#include <iosfwd>

namespace scc::cc
{
    enum class TokenType
    {
        None,
        Identifier,
        Integer,
        FloatingPoint,
        String,
        Operator,
        Other,

        Comma,
        Semicolon,
        Colon,

        LeftParenthesis,
        RightParenthesis,

        LeftBrace,
        RightBrace,

        LeftBracket,
        RightBracket,

        // C90 keywords

        KeywordAuto,
        KeywordBreak,
        KeywordCase,
        KeywordChar,
        KeywordConst,
        KeywordContinue,
        KeywordDefault,
        KeywordDo,
        KeywordDouble,
        KeywordElse,
        KeywordEnum,
        KeywordExtern,
        KeywordFloat,
        KeywordFor,
        KeywordGoto,
        KeywordIf,
        KeywordInt,
        KeywordLong,
        KeywordRegister,
        KeywordReturn,
        KeywordShort,
        KeywordSigned,
        KeywordSizeof,
        KeywordStatic,
        KeywordStruct,
        KeywordSwitch,
        KeywordTypedef,
        KeywordUnion,
        KeywordUnsigned,
        KeywordVoid,
        KeywordVolatile,
        KeywordWhile,

        // C99 keywords

        KeywordInline,
        KeywordRestrict,
        KeywordBool,
        KeywordComplex,
        KeywordImaginary,

        // C11 keywords

        KeywordAlignas,
        KeywordAlignof,
        KeywordAtomic,
        KeywordGeneric,
        KeywordNoreturn,
        KeywordStaticAssert,
        KeywordThreadLocal,

        // C23 keywords

        KeywordConstexpr,
        KeywordFalse,
        KeywordNullptr,
        KeywordTrue,
        KeywordTypeof,
        KeywordTypeofUnqual,
        KeywordBitInt,
        KeywordDecimal32,
        KeywordDecimal64,
        KeywordDecimal128,
    };

    struct Token
    {
        TokenType Type{};
        std::string Raw, Value;
        uint64_t IntegerValue{};
        long double FloatingPointValue{};
    };

    class Parser
    {
    public:
        explicit Parser(
            std::istream &stream,
            Context &context,
            Builder &builder);

        void Parse();

        [[nodiscard]] toolkit::result<NodePtr> ParseNode();

        [[nodiscard]] toolkit::result<NodePtr> ParseFunctionNode(bool is_extern, QualifiedType result, std::string name);
        [[nodiscard]] toolkit::result<NodePtr> ParseVariableNode(bool is_extern, QualifiedType type, std::string name);

        [[nodiscard]] toolkit::result<NodePtr> ParseTypeDefNode();

        [[nodiscard]] toolkit::result<StatementNodePtr> ParseStatementNode(bool ignore_end = false);
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseSequenceStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseIfStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseWhileStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseDoWhileStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseForStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseReturnStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseBreakStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseContinueStatementNode();
        [[nodiscard]] toolkit::result<StatementNodePtr> ParseVariableStatementNode(bool ignore_end);

        [[nodiscard]] toolkit::result<ExpressionNodePtr> ParseExpressionNode();
        [[nodiscard]] toolkit::result<ExpressionNodePtr> ParseBinaryExpressionNode();
        [[nodiscard]] toolkit::result<ExpressionNodePtr> ParseBinaryExpressionNode(
            ExpressionNodePtr left,
            size_t min_precedence);
        [[nodiscard]] toolkit::result<ExpressionNodePtr> ParseOperandExpressionNode();
        [[nodiscard]] toolkit::result<ExpressionNodePtr> ParsePrimaryExpressionNode();

        [[nodiscard]] toolkit::result<QualifiedType> ParseType();
        [[nodiscard]] toolkit::result<QualifiedType> ParseBaseType();
        [[nodiscard]] toolkit::result<QualifiedType> ParseStructType(bool constness);
        [[nodiscard]] toolkit::result<QualifiedType> ParseUnionType(bool constness);
        [[nodiscard]] toolkit::result<QualifiedType> ParseEnumType(bool constness);

        [[nodiscard]] bool CouldBeType() const;

        [[nodiscard]] bool At(TokenType type) const;
        [[nodiscard]] bool At(TokenType type, std::string_view value) const;

        [[nodiscard]] toolkit::result<Token> Expect(TokenType type);
        [[nodiscard]] toolkit::result<> Expect(TokenType type, std::string_view value);

        bool Skip(TokenType type);
        bool Skip(TokenType type, std::string_view value);

        Token Skip();
        Token Next();

    private:
        std::istream &m_Stream;
        Context &m_Context;
        Builder &m_Builder;

        int m_Buffer;
        Token m_Token;
    };
}

template<>
struct std::formatter<scc::cc::TokenType>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    auto format(const scc::cc::TokenType value, C &&ctx) const
    {
        static const std::unordered_map<scc::cc::TokenType, const char *> map
        {
            { scc::cc::TokenType::None, "None" },
            { scc::cc::TokenType::Identifier, "Identifier" },
            { scc::cc::TokenType::Integer, "Integer" },
            { scc::cc::TokenType::FloatingPoint, "FloatingPoint" },
            { scc::cc::TokenType::String, "String" },
            { scc::cc::TokenType::Operator, "Operator" },
            { scc::cc::TokenType::Other, "Other" },
        };

        if (const auto it = map.find(value); it != map.end())
            return std::format_to(ctx.out(), "{}", it->second);

        return ctx.out();
    }
};

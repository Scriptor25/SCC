#pragma once

#include <scc/cc/node.hpp>

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
        explicit Parser(std::istream &stream, Context &context, Module &module);

        void Parse();

        std::unique_ptr<Node> ParseNode();

        std::unique_ptr<FunctionNode> ParseFunctionNode(Type *result, std::string name);
        std::unique_ptr<VariableNode> ParseVariableNode(Type *type, std::string name);

        std::unique_ptr<TypeDefNode> ParseTypeDefNode();

        std::unique_ptr<StatementNode> ParseStatementNode();
        std::unique_ptr<ExpressionNode> ParseExpressionNode();

        Type *ParseType();
        Type *ParseBaseType();
        Type *ParseStructType();
        Type *ParseUnionType();
        Type *ParseEnumType();

        [[nodiscard]] bool At(TokenType type) const;
        [[nodiscard]] bool At(TokenType type, std::string_view value) const;

        Token Expect(TokenType type);
        void Expect(TokenType type, std::string_view value);

        bool Skip(TokenType type);
        bool Skip(TokenType type, std::string_view value);

        Token Skip();

        Token Next();

    private:
        std::istream &m_Stream;
        Context &m_Context;
        Module &m_Module;

        int m_Buffer;
        Token m_Token;
    };
}

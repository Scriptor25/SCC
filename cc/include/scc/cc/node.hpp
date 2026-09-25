#pragma once

#include <cstdint>
#include <scc/cc/cc.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace scc::cc
{
    struct Node
    {
        virtual ~Node() = default;

        virtual void Generate() const = 0;
    };

    using NodePtr = std::unique_ptr<Node>;

    struct StatementNode : Node
    {
    };

    using StatementNodePtr = std::unique_ptr<StatementNode>;

    struct ExpressionNode : Node
    {
        [[nodiscard]] virtual int64_t EvaluateConstantInteger() const = 0;
    };

    using ExpressionNodePtr = std::unique_ptr<ExpressionNode>;

    struct FunctionArgument
    {
        Type *Ty{};
        std::optional<std::string> Name;
    };

    struct FunctionNode : Node
    {
        explicit FunctionNode(
            Type *result,
            std::string name,
            std::vector<FunctionArgument> arguments);
        explicit FunctionNode(
            Type *result,
            std::string name,
            std::vector<FunctionArgument> arguments,
            std::unique_ptr<StatementNode> content);

        void Generate() const override;

        // TODO: tags
        // TODO: attributes
        // TODO: modifiers

        Type *Result;
        std::string Name;
        std::vector<FunctionArgument> Arguments;
        std::unique_ptr<StatementNode> Content;
    };

    struct VariableNode : Node
    {
        explicit VariableNode(Type *type, std::string name);
        explicit VariableNode(Type *type, std::string name, ExpressionNodePtr value);

        void Generate() const override;

        // TODO: tags
        // TODO: attributes
        // TODO: modifiers

        Type *Ty;
        std::string Name;
        ExpressionNodePtr Value;
    };

    struct TypeDefNode : Node
    {
        explicit TypeDefNode(Type *type, std::string name);

        void Generate() const override;

        Type *Ty;
        std::string Name;
    };

    struct SequenceStatementNode : StatementNode
    {
        explicit SequenceStatementNode(std::vector<StatementNodePtr> nodes);

        void Generate() const override;

        std::vector<StatementNodePtr> Nodes;
    };

    struct VariableStatementNode : StatementNode
    {
        explicit VariableStatementNode(
            Type *type,
            std::vector<std::pair<std::string, ExpressionNodePtr>> elements);

        void Generate() const override;

        Type *Ty;
        std::vector<std::pair<std::string, ExpressionNodePtr>> Elements;
    };

    struct SymbolExpressionNode : ExpressionNode
    {
        explicit SymbolExpressionNode(std::string name);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        std::string Name;
    };

    struct IntegerExpressionNode : ExpressionNode
    {
        explicit IntegerExpressionNode(uint64_t value);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        uint64_t Value;
    };

    struct FloatingPointExpressionNode : ExpressionNode
    {
        explicit FloatingPointExpressionNode(long double value);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        long double Value;
    };

    struct StringExpressionNode : ExpressionNode
    {
        explicit StringExpressionNode(std::string value);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        std::string Value;
    };

    struct SizeOfTypeExpressionNode : ExpressionNode
    {
        explicit SizeOfTypeExpressionNode(Type *type);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        Type *Ty;
    };

    struct SizeOfValueExpressionNode : ExpressionNode
    {
        explicit SizeOfValueExpressionNode(ExpressionNodePtr value);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Value;
    };

    struct CastExpressionNode : ExpressionNode
    {
        explicit CastExpressionNode(Type *type, ExpressionNodePtr operand);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        Type *Ty;
        ExpressionNodePtr Operand;
    };

    struct CallExpressionNode : ExpressionNode
    {
        explicit CallExpressionNode(ExpressionNodePtr callee, std::vector<ExpressionNodePtr> arguments);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Callee;
        std::vector<ExpressionNodePtr> Arguments;
    };

    struct SubscriptExpressionNode : ExpressionNode
    {
        explicit SubscriptExpressionNode(ExpressionNodePtr value, ExpressionNodePtr index);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Value;
        ExpressionNodePtr Index;
    };

    struct MemberExpressionNode : ExpressionNode
    {
        explicit MemberExpressionNode(ExpressionNodePtr value, std::string name, bool indirect);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Value;
        std::string Name;
        bool Indirect;
    };

    enum class UnaryOperator
    {
        Positive,
        Negative,
        Not,
        LogicalNot,
        Dereference,
        Reference,
        PrefixIncrement,
        PrefixDecrement,
        SuffixIncrement,
        SuffixDecrement,
    };

    struct UnaryExpressionNode : ExpressionNode
    {
        explicit UnaryExpressionNode(UnaryOperator operator_, ExpressionNodePtr operand);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        UnaryOperator Operator;
        ExpressionNodePtr Operand;
    };

    enum class BinaryOperator
    {
        Special,

        Add,
        Subtract,
        Multiply,
        Divide,
        Remainder,
        ShiftLeft,
        ShiftRight,
        And,
        XOr,
        Or,
        LogicalAnd,
        LogicalOr,

        CompareEqual,
        CompareNotEqual,
        CompareLessThan,
        CompareLessThanEqual,
        CompareGreaterThen,
        CompareGreaterThenEqual,

        Assign,
        AddAssign,
        SubtractAssign,
        MultiplyAssign,
        DivideAssign,
        RemainderAssign,
        ShiftLeftAssign,
        ShiftRightAssign,
        AndAssign,
        XOrAssign,
        OrAssign,
    };

    struct BinaryExpressionNode : ExpressionNode
    {
        explicit BinaryExpressionNode(
            BinaryOperator operator_,
            ExpressionNodePtr left,
            ExpressionNodePtr right);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        BinaryOperator Operator;
        ExpressionNodePtr Left;
        ExpressionNodePtr Right;
    };

    struct TernaryExpressionNode : ExpressionNode
    {
        explicit TernaryExpressionNode(
            ExpressionNodePtr condition,
            ExpressionNodePtr then,
            ExpressionNodePtr else_);

        void Generate() const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Condition;
        ExpressionNodePtr Then;
        ExpressionNodePtr Else;
    };
}

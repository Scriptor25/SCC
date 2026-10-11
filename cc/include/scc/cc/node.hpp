#pragma once

#include <scc/cc/cc.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace scc::cc
{
    struct Node
    {
        virtual ~Node() = default;

        virtual void Generate(Builder &builder) const = 0;
    };

    struct StatementNode : Node
    {
    };

    struct ExpressionNode : Node
    {
        [[nodiscard]] virtual QualifiedType GetType(Builder &builder) const = 0;

        [[nodiscard]] virtual const Value *GenerateValue(Builder &builder) const = 0;

        [[nodiscard]] virtual int64_t EvaluateConstantInteger() const = 0;
    };

    struct FunctionArgument
    {
        QualifiedType Ty;
        std::optional<std::string> Name;
    };

    struct FunctionNode : Node
    {
        explicit FunctionNode(
            bool is_extern,
            QualifiedType result,
            std::string name,
            std::vector<FunctionArgument> arguments,
            bool variadic);
        explicit FunctionNode(
            bool is_extern,
            QualifiedType result,
            std::string name,
            std::vector<FunctionArgument> arguments,
            bool variadic,
            std::unique_ptr<StatementNode> content);

        void Generate(Builder &builder) const override;

        // TODO: tags
        // TODO: attributes
        // TODO: modifiers

        bool Extern;
        QualifiedType Result;
        std::string Name;
        std::vector<FunctionArgument> Arguments;
        bool Variadic;
        std::unique_ptr<StatementNode> Content;
    };

    struct VariableNode : Node
    {
        explicit VariableNode(
            bool is_extern,
            QualifiedType ty,
            std::string name);
        explicit VariableNode(
            bool is_extern,
            QualifiedType ty,
            std::string name,
            ExpressionNodePtr value);

        void Generate(Builder &builder) const override;

        // TODO: tags
        // TODO: attributes
        // TODO: modifiers

        bool Extern;
        QualifiedType Ty;
        std::string Name;
        ExpressionNodePtr Value;
    };

    struct TypeDefNode : Node
    {
        explicit TypeDefNode(QualifiedType ty, std::string name);

        void Generate(Builder &builder) const override;

        QualifiedType Ty;
        std::string Name;
    };

    struct ExpressionStatementNode : StatementNode
    {
        explicit ExpressionStatementNode(ExpressionNodePtr value);

        void Generate(Builder &builder) const override;

        ExpressionNodePtr Value;
    };

    struct IfStatementNode : StatementNode
    {
        explicit IfStatementNode(
            ExpressionNodePtr condition,
            StatementNodePtr then,
            StatementNodePtr else_);

        void Generate(Builder &builder) const override;

        ExpressionNodePtr Condition;
        StatementNodePtr Then;
        StatementNodePtr Else;
    };

    struct WhileStatementNode : StatementNode
    {
        explicit WhileStatementNode(
            ExpressionNodePtr condition,
            StatementNodePtr loop);

        void Generate(Builder &builder) const override;

        ExpressionNodePtr Condition;
        StatementNodePtr Loop;
    };

    struct DoWhileStatementNode : StatementNode
    {
        explicit DoWhileStatementNode(
            StatementNodePtr loop,
            ExpressionNodePtr condition);

        void Generate(Builder &builder) const override;

        StatementNodePtr Loop;
        ExpressionNodePtr Condition;
    };

    struct ForStatementNode : StatementNode
    {
        explicit ForStatementNode(
            StatementNodePtr prefix,
            ExpressionNodePtr condition,
            StatementNodePtr suffix,
            StatementNodePtr loop);

        void Generate(Builder &builder) const override;

        StatementNodePtr Prefix;
        ExpressionNodePtr Condition;
        StatementNodePtr Suffix;
        StatementNodePtr Loop;
    };

    struct ReturnStatementNode : StatementNode
    {
        explicit ReturnStatementNode(ExpressionNodePtr value);

        void Generate(Builder &builder) const override;

        ExpressionNodePtr Value;
    };

    struct BreakStatementNode : StatementNode
    {
        explicit BreakStatementNode() = default;

        void Generate(Builder &builder) const override;
    };

    struct ContinueStatementNode : StatementNode
    {
        explicit ContinueStatementNode() = default;

        void Generate(Builder &builder) const override;
    };

    struct SequenceStatementNode : StatementNode
    {
        explicit SequenceStatementNode(std::vector<StatementNodePtr> nodes);

        void Generate(Builder &builder) const override;

        std::vector<StatementNodePtr> Nodes;
    };

    struct VariableElement
    {
        std::string Name;
        ExpressionNodePtr Val;
    };

    struct VariableStatementNode : StatementNode
    {
        explicit VariableStatementNode(
            QualifiedType ty,
            std::vector<VariableElement> elements,
            bool is_volatile);

        void Generate(Builder &builder) const override;

        QualifiedType Ty;
        std::vector<VariableElement> Elements;
        bool Volatile;
    };

    struct SymbolExpressionNode : ExpressionNode
    {
        explicit SymbolExpressionNode(std::string name);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        std::string Name;
    };

    struct IntegerExpressionNode : ExpressionNode
    {
        explicit IntegerExpressionNode(uint64_t val);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        uint64_t Val;
    };

    struct FloatingPointExpressionNode : ExpressionNode
    {
        explicit FloatingPointExpressionNode(long double val);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        long double Val;
    };

    struct StringExpressionNode : ExpressionNode
    {
        explicit StringExpressionNode(std::string val);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        std::string Val;
    };

    struct SizeOfTypeExpressionNode : ExpressionNode
    {
        explicit SizeOfTypeExpressionNode(QualifiedType ty);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        QualifiedType Ty;
    };

    struct SizeOfValueExpressionNode : ExpressionNode
    {
        explicit SizeOfValueExpressionNode(ExpressionNodePtr val);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Val;
    };

    struct CastExpressionNode : ExpressionNode
    {
        explicit CastExpressionNode(QualifiedType ty, ExpressionNodePtr operand);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        QualifiedType Ty;
        ExpressionNodePtr Operand;
    };

    struct CallExpressionNode : ExpressionNode
    {
        explicit CallExpressionNode(ExpressionNodePtr callee, std::vector<ExpressionNodePtr> arguments);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Callee;
        std::vector<ExpressionNodePtr> Arguments;
    };

    struct SubscriptExpressionNode : ExpressionNode
    {
        explicit SubscriptExpressionNode(ExpressionNodePtr base, ExpressionNodePtr index);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Base;
        ExpressionNodePtr Index;
    };

    struct MemberExpressionNode : ExpressionNode
    {
        explicit MemberExpressionNode(ExpressionNodePtr base, std::string name, bool indirect);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Base;
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

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        UnaryOperator Operator;
        ExpressionNodePtr Operand;
    };

    enum class BinaryOperator
    {
        Special,

        Add,
        Sub,
        Mul,
        Div,
        Rem,
        Shl,
        Shr,
        And,
        Xor,
        Or,

        Assign,

        AddAssign,
        SubAssign,
        MulAssign,
        DivAssign,
        RemAssign,
        ShlAssign,
        ShrAssign,
        AndAssign,
        XorAssign,
        OrAssign,

        LAnd,
        LOr,

        CmpEq,
        CmpNe,
        CmpLt,
        CmpLe,
        CmpGt,
        CmpGe,
    };

    struct BinaryExpressionNode : ExpressionNode
    {
        explicit BinaryExpressionNode(
            BinaryOperator operator_,
            ExpressionNodePtr left,
            ExpressionNodePtr right);

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

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

        void Generate(Builder &builder) const override;

        [[nodiscard]] QualifiedType GetType(Builder &builder) const override;

        [[nodiscard]] const Value *GenerateValue(Builder &builder) const override;

        [[nodiscard]] int64_t EvaluateConstantInteger() const override;

        ExpressionNodePtr Condition;
        ExpressionNodePtr Then;
        ExpressionNodePtr Else;
    };
}

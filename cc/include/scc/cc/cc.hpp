#pragma once

#include <memory>

namespace scc::cc
{
    class Context;
    class Parser;
    class Builder;

    struct Type;
    using TypePtr = std::unique_ptr<Type>;

    struct QualifiedType
    {
        bool operator==(const QualifiedType &other) const;

        explicit operator bool() const;

        const Type *operator->() const;
        const Type *operator*() const;

        const Type *Base{};
        bool Const{};
    };

    struct Node;
    struct StatementNode;
    struct ExpressionNode;

    using NodePtr = std::unique_ptr<Node>;
    using StatementNodePtr = std::unique_ptr<StatementNode>;
    using ExpressionNodePtr = std::unique_ptr<ExpressionNode>;

    struct Value;
    using ValuePtr = std::unique_ptr<Value>;
}

#pragma once

#include <memory>

namespace scc::cc
{
    class Context;
    class Parser;
    class Builder;

    struct Type;
    using TypePtr = std::unique_ptr<Type>;

    struct Node;
    struct StatementNode;
    struct ExpressionNode;

    using NodePtr = std::unique_ptr<Node>;
    using StatementNodePtr = std::unique_ptr<StatementNode>;
    using ExpressionNodePtr = std::unique_ptr<ExpressionNode>;

    class Value;
    using ValuePtr = std::unique_ptr<Value>;
}

#pragma once

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

    struct StatementNode : Node
    {
    };

    struct ExpressionNode : Node
    {
        [[nodiscard]] virtual int64_t EvaluateConstantInteger() const = 0;
    };

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
        explicit VariableNode(Type *type, std::string name, std::unique_ptr<ExpressionNode> value);

        void Generate() const override;

        // TODO: tags
        // TODO: attributes
        // TODO: modifiers

        Type *Ty;
        std::string Name;
        std::unique_ptr<ExpressionNode> Value;
    };

    struct TypeDefNode : Node
    {
        explicit TypeDefNode(Type *type, std::string name);

        void Generate() const override;

        Type *Ty;
        std::string Name;
    };
}

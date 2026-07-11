#include <scc/cc/node.hpp>

#include <scc/assert.hpp>

scc::cc::FunctionNode::FunctionNode(Type *result, std::string name, std::vector<FunctionArgument> arguments)
    : Result(result),
      Name(std::move(name)),
      Arguments(std::move(arguments))
{
}

scc::cc::FunctionNode::FunctionNode(
    Type *result,
    std::string name,
    std::vector<FunctionArgument> arguments,
    std::unique_ptr<StatementNode> content)
    : Result(result),
      Name(std::move(name)),
      Arguments(std::move(arguments)),
      Content(std::move(content))
{
}

void scc::cc::FunctionNode::Generate() const
{
    Error("TODO");
}

scc::cc::VariableNode::VariableNode(Type *type, std::string name)
    : Ty(type),
      Name(std::move(name))
{
}

scc::cc::VariableNode::VariableNode(Type *type, std::string name, std::unique_ptr<ExpressionNode> value)
    : Ty(type),
      Name(std::move(name)),
      Value(std::move(value))
{
}

void scc::cc::VariableNode::Generate() const
{
    Error("TODO");
}

scc::cc::TypeDefNode::TypeDefNode(Type *type, std::string name)
    : Ty(type),
      Name(std::move(name))
{
}

void scc::cc::TypeDefNode::Generate() const
{
    Error("TODO");
}

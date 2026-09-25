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

scc::cc::VariableNode::VariableNode(Type *type, std::string name, ExpressionNodePtr value)
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

scc::cc::SequenceStatementNode::SequenceStatementNode(std::vector<StatementNodePtr> nodes)
    : Nodes(std::move(nodes))
{
}

void scc::cc::SequenceStatementNode::Generate() const
{
    Error("TODO");
}

scc::cc::VariableStatementNode::VariableStatementNode(
    Type *type,
    std::vector<std::pair<std::string, ExpressionNodePtr>> elements)
    : Ty(type),
      Elements(std::move(elements))
{
}

void scc::cc::VariableStatementNode::Generate() const
{
    Error("TODO");
}

scc::cc::SymbolExpressionNode::SymbolExpressionNode(std::string name)
    : Name(std::move(name))
{
}

void scc::cc::SymbolExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::SymbolExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::IntegerExpressionNode::IntegerExpressionNode(uint64_t value)
    : Value(value)
{
}

void scc::cc::IntegerExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::IntegerExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::FloatingPointExpressionNode::FloatingPointExpressionNode(long double value)
    : Value(value)
{
}

void scc::cc::FloatingPointExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::FloatingPointExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::StringExpressionNode::StringExpressionNode(std::string value)
    : Value(std::move(value))
{
}

void scc::cc::StringExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::StringExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::SizeOfTypeExpressionNode::SizeOfTypeExpressionNode(Type *type)
    : Ty(type)
{
}

void scc::cc::SizeOfTypeExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::SizeOfTypeExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::SizeOfValueExpressionNode::SizeOfValueExpressionNode(ExpressionNodePtr value)
    : Value(std::move(value))
{
}

void scc::cc::SizeOfValueExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::SizeOfValueExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::CastExpressionNode::CastExpressionNode(Type *type, ExpressionNodePtr operand)
    : Ty(type),
      Operand(std::move(operand))
{
}

void scc::cc::CastExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::CastExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::CallExpressionNode::CallExpressionNode(ExpressionNodePtr callee, std::vector<ExpressionNodePtr> arguments)
    : Callee(std::move(callee)),
      Arguments(std::move(arguments))
{
}

void scc::cc::CallExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::CallExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::SubscriptExpressionNode::SubscriptExpressionNode(ExpressionNodePtr value, ExpressionNodePtr index)
    : Value(std::move(value)),
      Index(std::move(index))
{
}

void scc::cc::SubscriptExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::SubscriptExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::BinaryExpressionNode::BinaryExpressionNode(
    const BinaryOperator operator_,
    ExpressionNodePtr left,
    ExpressionNodePtr right)
    : Operator(operator_),
      Left(std::move(left)),
      Right(std::move(right))
{
}

void scc::cc::BinaryExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::BinaryExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::TernaryExpressionNode::TernaryExpressionNode(
    ExpressionNodePtr condition,
    ExpressionNodePtr then,
    ExpressionNodePtr else_)
    : Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

void scc::cc::TernaryExpressionNode::Generate() const
{
    Error("TODO");
}

int64_t scc::cc::TernaryExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

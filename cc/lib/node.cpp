#include <scc/cc/builder.hpp>
#include <scc/cc/node.hpp>
#include <scc/cc/type.hpp>
#include <scc/cc/value.hpp>

#include <scc/ir/function.hpp>
#include <scc/ir/variable.hpp>

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

void scc::cc::FunctionNode::Generate(Builder &builder) const
{
    auto *result = Result->Generate(builder);

    std::vector<ir::Type *> arguments(Arguments.size());
    for (size_t i = 0; i < arguments.size(); ++i)
        arguments[i] = Arguments[i].Ty->Generate(builder);

    auto *type = builder.GetContext().GetFunctionType(result, arguments, false);

    ir::Function *function;
    if (auto *symbol = builder.GetModule().GetSymbol(Name))
    {
        if (symbol->GetType() != type)
            Error("function declaration mismatch");

        function = dynamic_cast<ir::Function *>(symbol);
    }
    else
    {
        function = builder.GetModule().CreateFunction(type, Name);

        builder.SetNamed(Name, Value::CreateR(TODO, function));
    }

    if (!Content)
        return;

    auto *entry_block = builder.GetBuilder().GetOrCreateBlock(function, "entry");
    builder.GetBuilder().SetInsertBlock(entry_block);

    builder.PushFrame(nullptr, nullptr);

    for (size_t i = 0; i < function->GetArgumentCount(); ++i)
        if (const auto &name = Arguments[i].Name)
        {
            auto *argument = function->GetArgument(i);

            argument->SetName(*name);

            // TODO: insert alloc at function start
            auto *argument_pointer = builder.GetBuilder().CreateAlloc(argument->GetType());
            auto argument_value = Value::CreateL(TODO, argument_pointer);

            builder.SetNamed(*name, std::move(argument_value));
        }

    Content->Generate(builder);

    builder.PopFrame();

    if (!builder.GetBuilder().GetInsertBlock()->GetTerminator())
    {
        if (dynamic_cast<VoidType *>(Result))
            builder.GetBuilder().CreateReturn();
        else
            Error("missing return statement");
    }

    builder.GetBuilder().ClearInsertBlock();
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

void scc::cc::VariableNode::Generate(Builder &builder) const
{
    // TODO: extern variables

    auto *type = Ty->Generate(builder);
    const auto *value = Value->GenerateValue(builder);

    ir::Constant *initializer;
    if (value)
    {
        initializer = dynamic_cast<ir::Constant *>(value->Load(builder));
        if (!initializer)
            Error("invalid variable initializer");
    }
    else
    {
        initializer = nullptr;
    }

    auto *variable = builder.GetModule().CreateVariable(type, Name, initializer);

    builder.SetNamed(Name, Value::CreateL(TODO, variable));
}

scc::cc::TypeDefNode::TypeDefNode(Type *type, std::string name)
    : Ty(type),
      Name(std::move(name))
{
}

void scc::cc::TypeDefNode::Generate(Builder &) const
{
    // noop
}

scc::cc::ExpressionStatementNode::ExpressionStatementNode(ExpressionNodePtr value)
    : Value(std::move(value))
{
}

void scc::cc::ExpressionStatementNode::Generate(Builder &builder) const
{
    Value->Generate(builder);
}

scc::cc::IfStatementNode::IfStatementNode(
    ExpressionNodePtr condition,
    StatementNodePtr then,
    StatementNodePtr else_)
    : Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

void scc::cc::IfStatementNode::Generate(Builder &builder) const
{
    auto *function = builder.GetBuilder().GetInsertFunction();
    auto *then_block = builder.GetBuilder().GetOrCreateBlock(function, "then");
    auto *else_block = builder.GetBuilder().GetOrCreateBlock(function, "else");
    auto *tail_block = builder.GetBuilder().GetOrCreateBlock(function, "tail");

    const auto *condition = Condition->GenerateValue(builder);

    builder.GetBuilder().CreateBranch(condition->Load(builder), then_block, else_block);

    builder.GetBuilder().SetInsertBlock(then_block);
    Then->Generate(builder);
    if (!builder.GetBuilder().GetInsertBlock()->GetTerminator())
        builder.GetBuilder().CreateBranch(tail_block);

    builder.GetBuilder().SetInsertBlock(else_block);
    if (Else)
        Else->Generate(builder);
    if (!builder.GetBuilder().GetInsertBlock()->GetTerminator())
        builder.GetBuilder().CreateBranch(tail_block);

    builder.GetBuilder().SetInsertBlock(tail_block);
}

scc::cc::WhileStatementNode::WhileStatementNode(ExpressionNodePtr condition, StatementNodePtr loop)
    : Condition(std::move(condition)),
      Loop(std::move(loop))
{
}

void scc::cc::WhileStatementNode::Generate(Builder &builder) const
{
    auto *function = builder.GetBuilder().GetInsertFunction();
    auto *head_block = builder.GetBuilder().GetOrCreateBlock(function, "head");
    auto *loop_block = builder.GetBuilder().GetOrCreateBlock(function, "loop");
    auto *tail_block = builder.GetBuilder().GetOrCreateBlock(function, "tail");

    builder.GetBuilder().CreateBranch(head_block);

    builder.GetBuilder().SetInsertBlock(head_block);
    const auto *condition = Condition->GenerateValue(builder);
    builder.GetBuilder().CreateBranch(condition->Load(builder), loop_block, tail_block);

    builder.GetBuilder().SetInsertBlock(loop_block);
    builder.PushFrame(head_block, tail_block);
    Loop->Generate(builder);
    builder.PopFrame();
    if (!builder.GetBuilder().GetInsertBlock()->GetTerminator())
        builder.GetBuilder().CreateBranch(head_block);

    builder.GetBuilder().SetInsertBlock(tail_block);
}

scc::cc::DoWhileStatementNode::DoWhileStatementNode(StatementNodePtr loop, ExpressionNodePtr condition)
    : Loop(std::move(loop)),
      Condition(std::move(condition))
{
}

void scc::cc::DoWhileStatementNode::Generate(Builder &builder) const
{
    auto *function = builder.GetBuilder().GetInsertFunction();
    auto *loop_block = builder.GetBuilder().GetOrCreateBlock(function, "loop");
    auto *head_block = builder.GetBuilder().GetOrCreateBlock(function, "head");
    auto *tail_block = builder.GetBuilder().GetOrCreateBlock(function, "tail");

    builder.GetBuilder().CreateBranch(loop_block);

    builder.GetBuilder().SetInsertBlock(loop_block);
    builder.PushFrame(head_block, tail_block);
    Loop->Generate(builder);
    builder.PopFrame();
    if (!builder.GetBuilder().GetInsertBlock()->GetTerminator())
        builder.GetBuilder().CreateBranch(head_block);

    builder.GetBuilder().SetInsertBlock(head_block);
    const auto *condition = Condition->GenerateValue(builder);
    builder.GetBuilder().CreateBranch(condition->Load(builder), loop_block, tail_block);

    builder.GetBuilder().SetInsertBlock(tail_block);
}

scc::cc::ForStatementNode::ForStatementNode(
    StatementNodePtr prefix,
    ExpressionNodePtr condition,
    StatementNodePtr suffix,
    StatementNodePtr loop)
    : Prefix(std::move(prefix)),
      Condition(std::move(condition)),
      Suffix(std::move(suffix)),
      Loop(std::move(loop))
{
}

void scc::cc::ForStatementNode::Generate(Builder &builder) const
{
    Error("TODO");
}

scc::cc::ReturnStatementNode::ReturnStatementNode(ExpressionNodePtr value)
    : Value(std::move(value))
{
}

void scc::cc::ReturnStatementNode::Generate(Builder &builder) const
{
    if (Value)
    {
        const auto *value = Value->GenerateValue(builder);
        builder.GetBuilder().CreateReturn(value->Load(builder));
    }
    else
    {
        builder.GetBuilder().CreateReturn();
    }
}

void scc::cc::BreakStatementNode::Generate(Builder &builder) const
{
    auto *tail_block = builder.GetTail();
    if (!tail_block)
        Error("missing frame tail");

    builder.GetBuilder().CreateBranch(tail_block);
}

void scc::cc::ContinueStatementNode::Generate(Builder &builder) const
{
    auto *head_block = builder.GetHead();
    if (!head_block)
        Error("missing frame head");

    builder.GetBuilder().CreateBranch(head_block);
}

scc::cc::SequenceStatementNode::SequenceStatementNode(std::vector<StatementNodePtr> nodes)
    : Nodes(std::move(nodes))
{
}

void scc::cc::SequenceStatementNode::Generate(Builder &builder) const
{
    builder.PushFrame(nullptr, nullptr);
    for (const auto &node : Nodes)
        node->Generate(builder);
    builder.PopFrame();
}

scc::cc::VariableStatementNode::VariableStatementNode(
    Type *type,
    std::vector<VariableElement> elements)
    : Ty(type),
      Elements(std::move(elements))
{
}

void scc::cc::VariableStatementNode::Generate(Builder &builder) const
{
    auto *ir_type = Ty->Generate(builder);

    for (const auto &element : Elements)
    {
        // TODO: insert alloc at function start
        auto *pointer = builder.GetBuilder().CreateAlloc(ir_type, 1);

        if (element.Val)
        {
            const auto *val = element.Val->GenerateValue(builder);

            builder.GetBuilder().CreateStore(pointer, val->Load(builder));
        }

        builder.SetNamed(element.Name, Value::CreateL(Ty, pointer));
    }
}

scc::cc::SymbolExpressionNode::SymbolExpressionNode(std::string name)
    : Name(std::move(name))
{
}

void scc::cc::SymbolExpressionNode::Generate(Builder &) const
{
    // noop
}

scc::cc::Type *scc::cc::SymbolExpressionNode::GetType(Builder &builder) const
{
    return builder.GetNamed(Name)->GetType();
}

int64_t scc::cc::SymbolExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::IntegerExpressionNode::IntegerExpressionNode(const uint64_t val)
    : Val(val)
{
}

void scc::cc::IntegerExpressionNode::Generate(Builder &) const
{
    // noop
}

int64_t scc::cc::IntegerExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::FloatingPointExpressionNode::FloatingPointExpressionNode(const long double val)
    : Val(val)
{
}

void scc::cc::FloatingPointExpressionNode::Generate(Builder &) const
{
    // noop
}

int64_t scc::cc::FloatingPointExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::StringExpressionNode::StringExpressionNode(std::string val)
    : Val(std::move(val))
{
}

void scc::cc::StringExpressionNode::Generate(Builder &) const
{
    // noop
}

int64_t scc::cc::StringExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::SizeOfTypeExpressionNode::SizeOfTypeExpressionNode(Type *ty)
    : Ty(ty)
{
}

void scc::cc::SizeOfTypeExpressionNode::Generate(Builder &) const
{
    // noop
}

int64_t scc::cc::SizeOfTypeExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::SizeOfValueExpressionNode::SizeOfValueExpressionNode(ExpressionNodePtr val)
    : Val(std::move(val))
{
}

void scc::cc::SizeOfValueExpressionNode::Generate(Builder &) const
{
    // noop
}

int64_t scc::cc::SizeOfValueExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::CastExpressionNode::CastExpressionNode(Type *ty, ExpressionNodePtr operand)
    : Ty(ty),
      Operand(std::move(operand))
{
}

void scc::cc::CastExpressionNode::Generate(Builder &) const
{
    // noop
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

void scc::cc::CallExpressionNode::Generate(Builder &builder) const
{
    (void) GenerateValue(builder);
}

scc::cc::Value *scc::cc::CallExpressionNode::GenerateValue(Builder &builder) const
{
    const auto *callee = Callee->GenerateValue(builder);

    std::vector<ir::Value *> arguments(Arguments.size());
    for (size_t i = 0; i < arguments.size(); ++i)
        arguments[i] = Arguments[i]->GenerateValue(builder)->Load(builder);

    auto *callee_value = callee->Load(builder);
    auto *callee_type = callee_value->GetType();

    while (const auto *ptr_type = dynamic_cast<ir::PointerType *>(callee_type))
        callee_type = ptr_type->GetElement();

    auto *callee_fn_type = dynamic_cast<ir::FunctionType *>(callee_type);
    if (!callee_fn_type)
        Error("invalid callee type");

    auto *value = builder.GetBuilder().CreateCall(callee_fn_type, callee_value, std::move(arguments));

    return builder.Manage(Value::CreateR(TODO, value));
}

int64_t scc::cc::CallExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::SubscriptExpressionNode::SubscriptExpressionNode(ExpressionNodePtr base, ExpressionNodePtr index)
    : Base(std::move(base)),
      Index(std::move(index))
{
}

void scc::cc::SubscriptExpressionNode::Generate(Builder &) const
{
    // noop
}

int64_t scc::cc::SubscriptExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::MemberExpressionNode::MemberExpressionNode(ExpressionNodePtr base, std::string name, const bool indirect)
    : Base(std::move(base)),
      Name(std::move(name)),
      Indirect(indirect)
{
}

void scc::cc::MemberExpressionNode::Generate(Builder &) const
{
    // noop
}

int64_t scc::cc::MemberExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::UnaryExpressionNode::UnaryExpressionNode(const UnaryOperator operator_, ExpressionNodePtr operand)
    : Operator(operator_),
      Operand(std::move(operand))
{
}

void scc::cc::UnaryExpressionNode::Generate(Builder &builder) const
{
    Error("TODO");
}

int64_t scc::cc::UnaryExpressionNode::EvaluateConstantInteger() const
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

void scc::cc::BinaryExpressionNode::Generate(Builder &builder) const
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

void scc::cc::TernaryExpressionNode::Generate(Builder &builder) const
{
    Error("TODO");
}

scc::cc::Value *scc::cc::TernaryExpressionNode::GenerateValue(Builder &builder) const
{
    auto *function = builder.GetBuilder().GetInsertFunction();
    auto *then_block = builder.GetBuilder().CreateBlock(function, "then");
    auto *else_block = builder.GetBuilder().CreateBlock(function, "else");
    auto *end_block = builder.GetBuilder().CreateBlock(function, "end");

    const auto *condition = Condition->GenerateValue(builder);
    builder.GetBuilder().CreateBranch(condition->Load(builder), then_block, else_block);

    builder.GetBuilder().SetInsertBlock(then_block);
    const auto *then_type = Then->GetType(builder);
    auto *then_value = Then->GenerateValue(builder)->Load(builder);
    auto *then_receiver_block = builder.GetBuilder().GetInsertBlock();
    auto *then_terminator = builder.GetBuilder().CreateBranch(end_block);

    builder.GetBuilder().SetInsertBlock(else_block);
    const auto *else_type = Then->GetType(builder);
    auto *else_value = Else->GenerateValue(builder)->Load(builder);
    auto *else_receiver_block = builder.GetBuilder().GetInsertBlock();
    auto *else_terminator = builder.GetBuilder().CreateBranch(end_block);

    builder.GetBuilder().SetInsertBlock(end_block);

    ir::Type *type;
    if (then_type == else_type)
    {
        type = then_type->Generate(builder);
    }
    else
    {
        Error("TODO");
    }

    auto *value = builder.GetBuilder().CreatePhi(
        type,
        {
            { then_receiver_block, then_value },
            { else_receiver_block, else_value },
        });

    return builder.Manage(Value::CreateR(TODO, value));
}

int64_t scc::cc::TernaryExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

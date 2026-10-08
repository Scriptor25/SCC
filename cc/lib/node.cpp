#include <scc/cc/builder.hpp>
#include <scc/cc/context.hpp>
#include <scc/cc/node.hpp>
#include <scc/cc/type.hpp>
#include <scc/cc/value.hpp>

#include <scc/ir/function.hpp>
#include <scc/ir/variable.hpp>

#include <scc/assert.hpp>

scc::cc::FunctionNode::FunctionNode(
    const bool is_extern,
    const Type *result,
    std::string name,
    std::vector<FunctionArgument> arguments,
    const bool variadic)
    : Extern(is_extern),
      Result(result),
      Name(std::move(name)),
      Arguments(std::move(arguments)),
      Variadic(variadic)
{
}

scc::cc::FunctionNode::FunctionNode(
    const bool is_extern,
    const Type *result,
    std::string name,
    std::vector<FunctionArgument> arguments,
    const bool variadic,
    std::unique_ptr<StatementNode> content)
    : Extern(is_extern),
      Result(result),
      Name(std::move(name)),
      Arguments(std::move(arguments)),
      Variadic(variadic),
      Content(std::move(content))
{
}

void scc::cc::FunctionNode::Generate(Builder &builder) const
{
    std::vector<const Type *> argument_types(Arguments.size());
    for (size_t i = 0; i < Arguments.size(); ++i)
        argument_types[i] = Arguments[i].Ty;

    auto *function_type = builder.GetContext().GetFunctionType(Result, std::move(argument_types), Variadic);
    auto *ir_function_type = function_type->Generate(builder);

    ir::Function *function;
    if (auto *symbol = builder.GetIRModule().GetSymbol(Name))
    {
        if (symbol->GetType() != ir_function_type)
            Error("function declaration mismatch");

        function = dynamic_cast<ir::Function *>(symbol);
    }
    else
    {
        function = builder.GetIRModule().CreateFunction(ir_function_type, Name);

        builder.SetNamed(Name, Value::CreateR(builder.GetContext().GetPointerType(function_type), function));
    }

    if (!Content)
        return;

    auto *entry_block = builder.GetIRBuilder().GetOrCreateBlock(function, "entry");
    builder.GetIRBuilder().SetInsertBlock(entry_block);

    builder.PushFrame(nullptr, nullptr);

    for (size_t i = 0; i < function->GetArgumentCount(); ++i)
        if (const auto &name = Arguments[i].Name)
        {
            auto *argument = function->GetArgument(i);

            argument->SetName(*name);

            auto *argument_pointer = builder.Allocate(argument->GetType(), 1);

            builder.GetIRBuilder().CreateStore(argument_pointer, argument, false);

            auto argument_value = Value::CreateL(Arguments[i].Ty, argument_pointer);

            builder.SetNamed(*name, std::move(argument_value));
        }

    Content->Generate(builder);

    builder.PopFrame();

    if (!builder.GetIRBuilder().GetInsertBlock()->GetTerminator())
    {
        if (dynamic_cast<const VoidType *>(Result))
            builder.GetIRBuilder().CreateReturn();
        else
            Error("missing return statement");
    }

    builder.GetIRBuilder().ClearInsertBlock();
}

scc::cc::VariableNode::VariableNode(
    const bool is_extern,
    const Type *type,
    std::string name)
    : Extern(is_extern),
      Ty(type),
      Name(std::move(name))
{
}

scc::cc::VariableNode::VariableNode(
    const bool is_extern,
    const Type *type,
    std::string name,
    ExpressionNodePtr value)
    : Extern(is_extern),
      Ty(type),
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

    auto *variable = builder.GetIRModule().CreateVariable(type, Name, initializer);

    builder.SetNamed(Name, Value::CreateL(Ty, variable));
}

scc::cc::TypeDefNode::TypeDefNode(const Type *type, std::string name)
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
    auto *function = builder.GetIRBuilder().GetInsertFunction();
    auto *then_block = builder.GetIRBuilder().GetOrCreateBlock(function, "then");
    auto *else_block = builder.GetIRBuilder().GetOrCreateBlock(function, "else");
    auto *tail_block = builder.GetIRBuilder().GetOrCreateBlock(function, "tail");

    const auto *condition = Condition->GenerateValue(builder);

    builder.GetIRBuilder().CreateBranch(condition->Load(builder), then_block, else_block);

    builder.GetIRBuilder().SetInsertBlock(then_block);
    Then->Generate(builder);
    if (!builder.GetIRBuilder().GetInsertBlock()->GetTerminator())
        builder.GetIRBuilder().CreateBranch(tail_block);

    builder.GetIRBuilder().SetInsertBlock(else_block);
    if (Else)
        Else->Generate(builder);
    if (!builder.GetIRBuilder().GetInsertBlock()->GetTerminator())
        builder.GetIRBuilder().CreateBranch(tail_block);

    builder.GetIRBuilder().SetInsertBlock(tail_block);
}

scc::cc::WhileStatementNode::WhileStatementNode(ExpressionNodePtr condition, StatementNodePtr loop)
    : Condition(std::move(condition)),
      Loop(std::move(loop))
{
}

void scc::cc::WhileStatementNode::Generate(Builder &builder) const
{
    auto *function = builder.GetIRBuilder().GetInsertFunction();
    auto *head_block = builder.GetIRBuilder().GetOrCreateBlock(function, "head");
    auto *loop_block = builder.GetIRBuilder().GetOrCreateBlock(function, "loop");
    auto *tail_block = builder.GetIRBuilder().GetOrCreateBlock(function, "tail");

    builder.GetIRBuilder().CreateBranch(head_block);

    builder.GetIRBuilder().SetInsertBlock(head_block);
    const auto *condition = Condition->GenerateValue(builder);
    builder.GetIRBuilder().CreateBranch(condition->Load(builder), loop_block, tail_block);

    builder.GetIRBuilder().SetInsertBlock(loop_block);
    builder.PushFrame(head_block, tail_block);
    Loop->Generate(builder);
    builder.PopFrame();
    if (!builder.GetIRBuilder().GetInsertBlock()->GetTerminator())
        builder.GetIRBuilder().CreateBranch(head_block);

    builder.GetIRBuilder().SetInsertBlock(tail_block);
}

scc::cc::DoWhileStatementNode::DoWhileStatementNode(StatementNodePtr loop, ExpressionNodePtr condition)
    : Loop(std::move(loop)),
      Condition(std::move(condition))
{
}

void scc::cc::DoWhileStatementNode::Generate(Builder &builder) const
{
    auto *function = builder.GetIRBuilder().GetInsertFunction();
    auto *loop_block = builder.GetIRBuilder().GetOrCreateBlock(function, "loop");
    auto *head_block = builder.GetIRBuilder().GetOrCreateBlock(function, "head");
    auto *tail_block = builder.GetIRBuilder().GetOrCreateBlock(function, "tail");

    builder.GetIRBuilder().CreateBranch(loop_block);

    builder.GetIRBuilder().SetInsertBlock(loop_block);
    builder.PushFrame(head_block, tail_block);
    Loop->Generate(builder);
    builder.PopFrame();
    if (!builder.GetIRBuilder().GetInsertBlock()->GetTerminator())
        builder.GetIRBuilder().CreateBranch(head_block);

    builder.GetIRBuilder().SetInsertBlock(head_block);
    const auto *condition = Condition->GenerateValue(builder);
    builder.GetIRBuilder().CreateBranch(condition->Load(builder), loop_block, tail_block);

    builder.GetIRBuilder().SetInsertBlock(tail_block);
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
        builder.GetIRBuilder().CreateReturn(value->Load(builder));
    }
    else
    {
        builder.GetIRBuilder().CreateReturn();
    }
}

void scc::cc::BreakStatementNode::Generate(Builder &builder) const
{
    auto *tail_block = builder.GetTail();
    if (!tail_block)
        Error("missing frame tail");

    builder.GetIRBuilder().CreateBranch(tail_block);
}

void scc::cc::ContinueStatementNode::Generate(Builder &builder) const
{
    auto *head_block = builder.GetHead();
    if (!head_block)
        Error("missing frame head");

    builder.GetIRBuilder().CreateBranch(head_block);
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
    const Type *type,
    std::vector<VariableElement> elements,
    const bool is_volatile)
    : Ty(type),
      Elements(std::move(elements)),
      Volatile(is_volatile)
{
}

void scc::cc::VariableStatementNode::Generate(Builder &builder) const
{
    auto *ir_type = Ty->Generate(builder);

    for (const auto &element : Elements)
    {
        auto *pointer = builder.Allocate(ir_type, 1);

        if (element.Val)
        {
            const auto *val = element.Val->GenerateValue(builder);

            builder.GetIRBuilder().CreateStore(pointer, val->Load(builder), Volatile);
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

const scc::cc::Type *scc::cc::SymbolExpressionNode::GetType(Builder &builder) const
{
    return builder.GetNamed(Name)->GetType();
}

const scc::cc::Value *scc::cc::SymbolExpressionNode::GenerateValue(Builder &builder) const
{
    return builder.GetNamed(Name);
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

const scc::cc::Type *scc::cc::IntegerExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::IntegerExpressionNode::GenerateValue(Builder &builder) const
{
    const auto *type = builder.GetContext().GetIntegerType(IntegerKind::SignedInt);
    auto *value = builder.GetIRContext().GetInt(type->Generate(builder), Val);

    return builder.Manage(Value::CreateR(type, value));
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

const scc::cc::Type *scc::cc::FloatingPointExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::FloatingPointExpressionNode::GenerateValue(Builder &builder) const
{
    Error("TODO");
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

const scc::cc::Type *scc::cc::StringExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::StringExpressionNode::GenerateValue(Builder &builder) const
{
    Error("TODO");
}

int64_t scc::cc::StringExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::SizeOfTypeExpressionNode::SizeOfTypeExpressionNode(const Type *ty)
    : Ty(ty)
{
}

void scc::cc::SizeOfTypeExpressionNode::Generate(Builder &) const
{
    // noop
}

const scc::cc::Type *scc::cc::SizeOfTypeExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::SizeOfTypeExpressionNode::GenerateValue(Builder &builder) const
{
    Error("TODO");
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

const scc::cc::Type *scc::cc::SizeOfValueExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::SizeOfValueExpressionNode::GenerateValue(Builder &builder) const
{
    Error("TODO");
}

int64_t scc::cc::SizeOfValueExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

scc::cc::CastExpressionNode::CastExpressionNode(const Type *ty, ExpressionNodePtr operand)
    : Ty(ty),
      Operand(std::move(operand))
{
}

void scc::cc::CastExpressionNode::Generate(Builder &) const
{
    // noop
}

const scc::cc::Type *scc::cc::CastExpressionNode::GetType(Builder &) const
{
    return Ty;
}

const scc::cc::Value *scc::cc::CastExpressionNode::GenerateValue(Builder &builder) const
{
    auto *type = Ty->Generate(builder);
    const auto *operand = Operand->GenerateValue(builder);

    auto *value = builder.GetIRBuilder().CreateCast(type, operand->Load(builder));

    return builder.Manage(Value::CreateR(Ty, value));
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

const scc::cc::Type *scc::cc::CallExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::CallExpressionNode::GenerateValue(Builder &builder) const
{
    const auto *callee = Callee->GenerateValue(builder);

    auto *callee_type = callee->GetType();
    while (const auto *ptr_type = dynamic_cast<const PointerType *>(callee_type))
        callee_type = ptr_type->Element;

    auto *callee_fn_type = dynamic_cast<const FunctionType *>(callee_type);
    if (!callee_fn_type)
        Error("invalid callee type");

    std::vector<ir::Value *> ir_arguments(Arguments.size());
    for (size_t i = 0; i < ir_arguments.size(); ++i)
        ir_arguments[i] = Arguments[i]->GenerateValue(builder)->Load(builder);

    auto *ir_callee_value = callee->Load(builder);

    auto *fn_type = callee_fn_type->Generate(builder);

    auto *value = builder.GetIRBuilder().CreateCall(fn_type, ir_callee_value, std::move(ir_arguments));

    return builder.Manage(Value::CreateR(callee_fn_type->Result, value));
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

const scc::cc::Type *scc::cc::SubscriptExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::SubscriptExpressionNode::GenerateValue(Builder &builder) const
{
    Error("TODO");
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

const scc::cc::Type *scc::cc::MemberExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::MemberExpressionNode::GenerateValue(Builder &builder) const
{
    Error("TODO");
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

const scc::cc::Type *scc::cc::UnaryExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::UnaryExpressionNode::GenerateValue(Builder &builder) const
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
    (void) GenerateValue(builder);
}

const scc::cc::Type *scc::cc::BinaryExpressionNode::GetType(Builder &builder) const
{
    Error("TODO");
}

const scc::cc::Value *scc::cc::BinaryExpressionNode::GenerateValue(Builder &builder) const
{
    switch (Operator)
    {
    case BinaryOperator::LogicalAnd:
    {
        auto *function = builder.GetIRBuilder().GetInsertFunction();
        auto *next_block = builder.GetIRBuilder().CreateBlock(function, "next");
        auto *end_block = builder.GetIRBuilder().CreateBlock(function, "end");

        auto *left_value = Left->GenerateValue(builder)->Load(builder);
        auto *left_block = builder.GetIRBuilder().GetInsertBlock();
        auto *condition = builder.GetIRBuilder().CreateNotNull(left_value);
        builder.GetIRBuilder().CreateBranch(condition, next_block, end_block);

        builder.GetIRBuilder().SetInsertBlock(next_block);
        auto *right_value = Right->GenerateValue(builder)->Load(builder);
        auto *right_block = builder.GetIRBuilder().GetInsertBlock();
        builder.GetIRBuilder().CreateBranch(end_block);

        builder.GetIRBuilder().SetInsertBlock(end_block);

        auto *type = builder.GetContext().GetBooleanType();
        auto *value = builder.GetIRBuilder().CreatePhi(
            type->Generate(builder),
            {
                { left_block, left_value },
                { right_block, right_value },
            });

        return builder.Manage(Value::CreateR(type, value));
    }
    case BinaryOperator::LogicalOr:
    {
        auto *function = builder.GetIRBuilder().GetInsertFunction();
        auto *next_block = builder.GetIRBuilder().CreateBlock(function, "next");
        auto *end_block = builder.GetIRBuilder().CreateBlock(function, "end");

        auto *left_value = Left->GenerateValue(builder)->Load(builder);
        auto *left_block = builder.GetIRBuilder().GetInsertBlock();
        auto *condition = builder.GetIRBuilder().CreateNotNull(left_value);
        builder.GetIRBuilder().CreateBranch(condition, end_block, next_block);

        builder.GetIRBuilder().SetInsertBlock(next_block);
        auto *right_value = Right->GenerateValue(builder)->Load(builder);
        auto *right_block = builder.GetIRBuilder().GetInsertBlock();
        builder.GetIRBuilder().CreateBranch(end_block);

        builder.GetIRBuilder().SetInsertBlock(end_block);

        auto *type = builder.GetContext().GetBooleanType();
        auto *value = builder.GetIRBuilder().CreatePhi(
            type->Generate(builder),
            {
                { left_block, left_value },
                { right_block, right_value },
            });

        return builder.Manage(Value::CreateR(type, value));
    }

    default:
        break;
    }

    const auto *left = Left->GenerateValue(builder);
    const auto *right = Right->GenerateValue(builder);

    const auto *left_type = left->GetType();
    const auto *right_type = right->GetType();

    auto *type = Type::Collapse(
        builder.GetContext(),
        left_type,
        right_type);
    Assert(type, "type must not be null");

    // TODO: handle pointer arithmetic

    auto *value_type = type->Generate(builder);

    auto *left_value = left->Load(builder);
    if (left_type != type)
        left_value = builder.GetIRBuilder().CreateCast(value_type, left_value);

    auto *right_value = right->Load(builder);
    if (right_type != type)
        right_value = builder.GetIRBuilder().CreateCast(value_type, right_value);

    std::vector operands
    {
        left_value,
        right_value,
    };

    ir::Value *value;
    switch (Operator)
    {
    case BinaryOperator::Assign:
        value = right_value;
        break;

    case BinaryOperator::Add:
    case BinaryOperator::AddAssign:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateIOperatorADD(value_type, std::move(operands));
        else if (dynamic_cast<const FloatingPointType *>(type))
            value = builder.GetIRBuilder().CreateFOperatorADD(value_type, std::move(operands));
        else
            value = nullptr;
        break;
    case BinaryOperator::Subtract:
    case BinaryOperator::SubtractAssign:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateIOperatorSUB(value_type, std::move(operands));
        else if (dynamic_cast<const FloatingPointType *>(type))
            value = builder.GetIRBuilder().CreateFOperatorSUB(value_type, std::move(operands));
        else
            value = nullptr;
        break;
    case BinaryOperator::Multiply:
    case BinaryOperator::MultiplyAssign:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateIOperatorMUL(value_type, std::move(operands));
        else if (dynamic_cast<const FloatingPointType *>(type))
            value = builder.GetIRBuilder().CreateFOperatorMUL(value_type, std::move(operands));
        else
            value = nullptr;
        break;
    case BinaryOperator::Divide:
    case BinaryOperator::DivideAssign:
        if (const auto *integer_type = dynamic_cast<const IntegerType *>(type))
        {
            if (integer_type->IsSigned())
                value = builder.GetIRBuilder().CreateIOperatorSDIV(value_type, std::move(operands));
            else
                value = builder.GetIRBuilder().CreateIOperatorUDIV(value_type, std::move(operands));
        }
        else if (dynamic_cast<const FloatingPointType *>(type))
            value = builder.GetIRBuilder().CreateFOperatorDIV(value_type, std::move(operands));
        else
            value = nullptr;
        break;
    case BinaryOperator::Remainder:
    case BinaryOperator::RemainderAssign:
        if (const auto *integer_type = dynamic_cast<const IntegerType *>(type))
        {
            if (integer_type->IsSigned())
                value = builder.GetIRBuilder().CreateIOperatorSREM(value_type, std::move(operands));
            else
                value = builder.GetIRBuilder().CreateIOperatorUREM(value_type, std::move(operands));
        }
        else if (dynamic_cast<const FloatingPointType *>(type))
            value = builder.GetIRBuilder().CreateFOperatorREM(value_type, std::move(operands));
        else
            value = nullptr;
        break;

    case BinaryOperator::ShiftLeft:
    case BinaryOperator::ShiftLeftAssign:
        Error("TODO");
    case BinaryOperator::ShiftRight:
    case BinaryOperator::ShiftRightAssign:
        Error("TODO");

    case BinaryOperator::And:
    case BinaryOperator::AndAssign:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateIOperatorAND(value_type, std::move(operands));
        else
            value = nullptr;
        break;
    case BinaryOperator::XOr:
    case BinaryOperator::XOrAssign:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateIOperatorXOR(value_type, std::move(operands));
        else
            value = nullptr;
        break;
    case BinaryOperator::Or:
    case BinaryOperator::OrAssign:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateIOperatorOR(value_type, std::move(operands));
        else
            value = nullptr;
        break;

    case BinaryOperator::CompareEqual:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateICompareEQU(value_type, left_value, right_value);
        else if (dynamic_cast<const FloatingPointType *>(type))
        {
            if (true)
                value = builder.GetIRBuilder().CreateFCompareOEQ(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateFCompareUEQ(value_type, left_value, right_value);
        }
        else
            value = nullptr;
        break;
    case BinaryOperator::CompareNotEqual:
        if (dynamic_cast<const IntegerType *>(type))
            value = builder.GetIRBuilder().CreateICompareNEQ(value_type, left_value, right_value);
        else if (dynamic_cast<const FloatingPointType *>(type))
        {
            if (true)
                value = builder.GetIRBuilder().CreateFCompareONE(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateFCompareUNE(value_type, left_value, right_value);
        }
        else
            value = nullptr;
        break;
    case BinaryOperator::CompareLessThan:
        if (const auto *integer_type = dynamic_cast<const IntegerType *>(type))
        {
            if (integer_type->IsSigned())
                value = builder.GetIRBuilder().CreateICompareSLT(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateICompareULT(value_type, left_value, right_value);
        }
        else if (dynamic_cast<const FloatingPointType *>(type))
        {
            if (true)
                value = builder.GetIRBuilder().CreateFCompareOLT(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateFCompareULT(value_type, left_value, right_value);
        }
        else
            value = nullptr;
        break;
    case BinaryOperator::CompareLessThanEqual:
        if (const auto *integer_type = dynamic_cast<const IntegerType *>(type))
        {
            if (integer_type->IsSigned())
                value = builder.GetIRBuilder().CreateICompareSLE(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateICompareULE(value_type, left_value, right_value);
        }
        else if (dynamic_cast<const FloatingPointType *>(type))
        {
            if (true)
                value = builder.GetIRBuilder().CreateFCompareOLE(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateFCompareULE(value_type, left_value, right_value);
        }
        else
            value = nullptr;
        break;
    case BinaryOperator::CompareGreaterThen:
        if (const auto *integer_type = dynamic_cast<const IntegerType *>(type))
        {
            if (integer_type->IsSigned())
                value = builder.GetIRBuilder().CreateICompareSGT(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateICompareUGT(value_type, left_value, right_value);
        }
        else if (dynamic_cast<const FloatingPointType *>(type))
        {
            if (true)
                value = builder.GetIRBuilder().CreateFCompareOGT(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateFCompareUGT(value_type, left_value, right_value);
        }
        else
            value = nullptr;
        break;
    case BinaryOperator::CompareGreaterThenEqual:
        if (const auto *integer_type = dynamic_cast<const IntegerType *>(type))
        {
            if (integer_type->IsSigned())
                value = builder.GetIRBuilder().CreateICompareSGE(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateICompareUGE(value_type, left_value, right_value);
        }
        else if (dynamic_cast<const FloatingPointType *>(type))
        {
            if (true)
                value = builder.GetIRBuilder().CreateFCompareOGE(value_type, left_value, right_value);
            else
                value = builder.GetIRBuilder().CreateFCompareUGE(value_type, left_value, right_value);
        }
        else
            value = nullptr;
        break;

    default:
        Error("TODO");
    }

    Assert(value, "value must not be null");

    switch (Operator)
    {
    case BinaryOperator::Assign:
    case BinaryOperator::AddAssign:
    case BinaryOperator::SubtractAssign:
    case BinaryOperator::MultiplyAssign:
    case BinaryOperator::DivideAssign:
    case BinaryOperator::RemainderAssign:
    case BinaryOperator::ShiftLeftAssign:
    case BinaryOperator::ShiftRightAssign:
    case BinaryOperator::AndAssign:
    case BinaryOperator::XOrAssign:
    case BinaryOperator::OrAssign:
        left->Store(builder.GetIRContext(), builder.GetIRBuilder(), value);
        return left;

    case BinaryOperator::CompareEqual:
    case BinaryOperator::CompareNotEqual:
    case BinaryOperator::CompareLessThan:
    case BinaryOperator::CompareLessThanEqual:
    case BinaryOperator::CompareGreaterThen:
    case BinaryOperator::CompareGreaterThenEqual:
        return builder.Manage(Value::CreateR(builder.GetContext().GetBooleanType(), value));

    default:
        break;
    }

    return builder.Manage(Value::CreateR(type, value));
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

const scc::cc::Type *scc::cc::TernaryExpressionNode::GetType(Builder &builder) const
{
    auto *then_type = Then->GetType(builder);
    auto *else_type = Else->GetType(builder);

    auto *type = Type::Collapse(builder.GetContext(), then_type, else_type);
    Assert(type, "failed to collapse ternary operation result types");

    return type;
}

const scc::cc::Value *scc::cc::TernaryExpressionNode::GenerateValue(Builder &builder) const
{
    auto *function = builder.GetIRBuilder().GetInsertFunction();
    auto *then_block = builder.GetIRBuilder().CreateBlock(function, "then");
    auto *else_block = builder.GetIRBuilder().CreateBlock(function, "else");
    auto *end_block = builder.GetIRBuilder().CreateBlock(function, "end");

    const auto *condition = Condition->GenerateValue(builder);
    builder.GetIRBuilder().CreateBranch(condition->Load(builder), then_block, else_block);

    builder.GetIRBuilder().SetInsertBlock(then_block);
    auto *then_type = Then->GetType(builder);
    auto *then_value = Then->GenerateValue(builder)->Load(builder);
    auto *then_receiver_block = builder.GetIRBuilder().GetInsertBlock();
    auto *then_terminator = builder.GetIRBuilder().CreateBranch(end_block);

    builder.GetIRBuilder().SetInsertBlock(else_block);
    auto *else_type = Else->GetType(builder);
    auto *else_value = Else->GenerateValue(builder)->Load(builder);
    auto *else_receiver_block = builder.GetIRBuilder().GetInsertBlock();
    auto *else_terminator = builder.GetIRBuilder().CreateBranch(end_block);

    auto *type = Type::Collapse(builder.GetContext(), then_type, else_type);
    Assert(type, "failed to collapse ternary operation result types");

    if (type != then_type)
    {
        builder.GetIRBuilder().SetInsertPoint(then_terminator);
        then_value = builder.GetIRBuilder().CreateCast(type->Generate(builder), then_value);
    }

    if (type != else_type)
    {
        builder.GetIRBuilder().SetInsertPoint(else_terminator);
        else_value = builder.GetIRBuilder().CreateCast(type->Generate(builder), else_value);
    }

    builder.GetIRBuilder().SetInsertBlock(end_block);

    auto *value = builder.GetIRBuilder().CreatePhi(
        type->Generate(builder),
        {
            { then_receiver_block, then_value },
            { else_receiver_block, else_value },
        });

    return builder.Manage(Value::CreateR(type, value));
}

int64_t scc::cc::TernaryExpressionNode::EvaluateConstantInteger() const
{
    Error("TODO");
}

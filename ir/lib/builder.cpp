#include <scc/ir/block.hpp>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/function.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/module.hpp>
#include <scc/ir/value.hpp>

#include <scc/assert.hpp>
#include <scc/error.hpp>

#include <ranges>

scc::ir::Builder::Builder(Context &context)
    : m_Context(context),
      m_InsertBlock()
{
}

scc::ir::Context &scc::ir::Builder::GetContext() const
{
    return m_Context;
}

scc::ir::Variable *scc::ir::Builder::CreateString(
    Module &module,
    std::string name,
    const std::string_view value) const
{
    std::string buffer;
    buffer += value;
    buffer += static_cast<char>(0);

    const auto initializer = m_Context.GetArray(buffer);
    const auto type = initializer->GetType();

    return module.CreateVariable(type, std::move(name), initializer);
}

scc::ir::Block *scc::ir::Builder::GetOrCreateBlock(Function *function, std::string name) const
{
    Assert(!!function, "function must not be null");
    Assert(!name.empty(), "name must not be empty");

    if (const auto block = function->FindBlock(name))
        return block;

    return function->CreateBlock(std::move(name));
}

void scc::ir::Builder::SetInsertBlock(Block *block)
{
    Assert(!!block, "block must not be null");

    m_InsertBlock = block;
}

void scc::ir::Builder::ClearInsertBlock()
{
    m_InsertBlock = nullptr;
}

scc::ir::Block *scc::ir::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

scc::ir::Function *scc::ir::Builder::GetInsertFunction() const
{
    Assert(!!m_InsertBlock, "insert block must not be null");

    return m_InsertBlock->GetFunction();
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateOperator(
    Operator operator_,
    std::vector<Value *> operands,
    std::string name)
{
    Assert(!operands.empty(), "operands must not be empty");

    auto type = operands.front()->GetType();

    for (const auto operand : operands)
        Assert(operand->GetType() == type, "non-uniform operands");

    return Create<OperatorInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        operator_,
        std::move(operands));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateAdd(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Add, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateSub(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Sub, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateMul(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Mul, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateSDiv(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::SDiv, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateUDiv(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::UDiv, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateSRem(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::SRem, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateURem(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::URem, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateAnd(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::And, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateOr(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Or, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateXor(
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Xor, std::move(operands), std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateComparator(
    Comparator comparator,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    Assert(!!lhs, "lhs must not be null");
    Assert(!!rhs, "rhs must not be null");
    Assert(lhs->GetType() == rhs->GetType(), "rhs type must be same as lhs type");

    return Create<ComparatorInstruction>(
        m_Context.GetInt1Type(),
        m_InsertBlock,
        std::move(name),
        comparator,
        lhs,
        rhs);
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSLT(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SLT, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateULT(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::ULT, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSGT(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SGT, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateUGT(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::UGT, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSLE(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SLE, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateULE(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::ULE, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSGE(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SGE, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateUGE(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::UGE, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateEQ(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::EQ, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateNE(
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::NE, lhs, rhs, std::move(name));
}

scc::ir::DirectBranchInstruction *scc::ir::Builder::CreateBranch(Block *destination)
{
    Assert(!!destination, "destination must not be null");

    return Create<DirectBranchInstruction>(
        m_Context.GetVoidType(),
        m_InsertBlock,
        destination);
}

scc::ir::BranchInstruction *scc::ir::Builder::CreateBranch(
    Value *condition,
    Block *then,
    Block *else_)
{
    Assert(!!condition, "condition must not be null");
    Assert(condition->GetType() == m_Context.GetInt1Type(), "condition type must be i1");
    Assert(!!then, "then must not be null");
    Assert(!!else_, "else must not be null");

    return Create<BranchInstruction>(
        m_Context.GetVoidType(),
        m_InsertBlock,
        condition,
        then,
        else_);
}

scc::ir::ReturnInstruction *scc::ir::Builder::CreateRet()
{
    return Create<ReturnInstruction>(m_Context.GetVoidType(), m_InsertBlock);
}

scc::ir::ReturnInstruction *scc::ir::Builder::CreateRet(Value *value)
{
    Assert(!!value, "value must not be null");

    return Create<ReturnInstruction>(m_Context.GetVoidType(), m_InsertBlock, value);
}

scc::ir::SelectInstruction *scc::ir::Builder::CreateSelect(
    std::vector<std::pair<Block *, Value *>> options,
    std::string name)
{
    Assert(!options.empty(), "options must not be empty");

    auto type = options.front().second->GetType();

    for (const auto value : options | std::views::values)
        Assert(value->GetType() == type, "non-uniform options");

    return Create<SelectInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        std::move(options));
}

scc::ir::AllocInstruction *scc::ir::Builder::CreateAlloc(Type *type, std::string name, unsigned count)
{
    Assert(!!type, "type must not be null");

    return Create<AllocInstruction>(
        m_Context.GetPointerType(type),
        m_InsertBlock,
        std::move(name),
        count);
}

scc::ir::LoadInstruction *scc::ir::Builder::CreateLoad(Value *pointer, std::string name)
{
    Assert(!!pointer, "pointer must not be null");
    Assert(pointer->GetType()->GetKind() == Kind::Pointer, "pointer type must be a kind of pointer");

    auto type = dynamic_cast<PointerType *>(pointer->GetType())->GetElement();

    return Create<LoadInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        pointer);
}

scc::ir::StoreInstruction *scc::ir::Builder::CreateStore(Value *pointer, Value *value)
{
    Assert(!!pointer, "pointer must not be null");
    Assert(!!value, "value must not be null");
    Assert(pointer->GetType()->GetKind() == Kind::Pointer, "pointer type must be a kind of pointer");

    const auto type = dynamic_cast<PointerType *>(pointer->GetType())->GetElement();
    Assert(value->GetType() == type, "value type must be the same as pointer base type");

    return Create<StoreInstruction>(
        m_Context.GetVoidType(),
        m_InsertBlock,
        pointer,
        value);
}

scc::ir::OffsetInstruction *scc::ir::Builder::CreateOffset(
    Type *type,
    Value *base,
    std::vector<Value *> offsets,
    std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!!base, "base must not be null");
    Assert(!offsets.empty(), "offsets must not be empty");

    Assert(type->GetKind() == Kind::Pointer, "type must be a kind of pointer");
    Assert(base->GetType()->GetKind() == Kind::Pointer, "base type must be a kind of pointer");

    for (const auto offset : offsets)
        Assert(offset->GetType()->GetKind() == Kind::Int, "offset type must be a kind of int");

    return Create<OffsetInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        base,
        std::move(offsets));
}

scc::ir::OffsetInstruction *scc::ir::Builder::CreateConstOffset(
    Value *base,
    const std::vector<unsigned> &offsets,
    std::string name)
{
    Assert(!!base, "base must not be null");
    Assert(!offsets.empty(), "offsets must not be empty");

    Assert(base->GetType()->GetKind() == Kind::Pointer, "base type must be a kind of pointer");

    auto type = base->GetType();
    std::vector<Value *> values(offsets.size());

    for (auto i = 0ull; i < offsets.size(); ++i)
    {
        auto &offset = offsets[i];
        Assert(offset < type->GetElementCount(), "constant offset out of bounds");

        type = type->GetElement(offset);
        values[i] = m_Context.GetInt64(offset);
    }

    return Create<OffsetInstruction>(
        m_Context.GetPointerType(type),
        m_InsertBlock,
        std::move(name),
        base,
        std::move(values));
}

scc::ir::CallInstruction *scc::ir::Builder::CreateCall(
    Value *callee,
    std::vector<Value *> arguments,
    std::string name)
{
    Assert(!!callee, "callee must not be null");
    Assert(!arguments.empty(), "arguments must not be empty");

    const auto callee_type = callee->GetType();

    Assert(callee_type->GetKind() == Kind::Pointer, "callee type must be a kind of pointer");

    const auto pointer_type = dynamic_cast<PointerType *>(callee_type);
    const auto element_type = pointer_type->GetElement();

    Assert(element_type->GetKind() == Kind::Function, "callee element type must be a kind of function");

    const auto function_type = dynamic_cast<FunctionType *>(element_type);

    const auto count = function_type->GetArgumentCount();

    Assert(count <= arguments.size(), "not enough arguments");
    Assert(function_type->IsVariadic() || count == arguments.size(), "too many arguments");

    for (auto i = 0ull; i < count; ++i)
        Assert(function_type->GetArgument(i) == arguments[i]->GetType(), "invalid argument type");

    return Create<CallInstruction>(
        function_type->GetResult(),
        m_InsertBlock,
        std::move(name),
        callee,
        std::move(arguments));
}

scc::ir::CastInstruction *scc::ir::Builder::CreateCast(
    Type *type,
    Value *value,
    std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!!value, "value must not be null");

    return Create<CastInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        value);
}

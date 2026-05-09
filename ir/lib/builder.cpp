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

static void assert_type_match(scc::ir::Type *a, scc::ir::Type *b)
{
    scc::Assert(a == b, "type mismatch: {} != {}", a, b);
}

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

    auto *initializer = m_Context.GetArray(buffer);
    auto *type = initializer->GetType();

    return module.CreateVariable(type, std::move(name), initializer);
}

scc::ir::Block *scc::ir::Builder::GetOrCreateBlock(Function *function, std::string name) const
{
    Assert(!!function, "function must not be null");
    Assert(!name.empty(), "name must not be empty");

    if (auto *block = function->FindBlock(name))
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

scc::ir::Type *scc::ir::Builder::GetInsertFunctionResult() const
{
    Assert(!!m_InsertBlock, "insert block must not be null");

    const auto *function = m_InsertBlock->GetFunction();
    const auto *function_type = dynamic_cast<FunctionType *>(function->GetType()->GetElement());

    return function_type->GetResult();
}

scc::ir::Value *scc::ir::Builder::CreateEmpty(Type *type, std::string name) const
{
    Assert(!!m_InsertBlock, "insert block must not be null");

    return m_InsertBlock->CreateEmpty(type, std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateOperator(
    Operator operator_,
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!operands.empty(), "operands must not be empty");

    for (const auto *operand : operands)
        assert_type_match(operand->GetType(), type);

    return Create<OperatorInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        operator_,
        std::move(operands));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateAdd(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Add, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateSub(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Sub, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateMul(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Mul, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateSDiv(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::SDiv, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateUDiv(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::UDiv, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateSRem(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::SRem, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateURem(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::URem, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateAnd(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::And, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateOr(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Or, type, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction *scc::ir::Builder::CreateXor(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateOperator(Operator::Xor, type, std::move(operands), std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateComparator(
    Comparator comparator,
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!!lhs, "lhs must not be null");
    Assert(!!rhs, "rhs must not be null");

    assert_type_match(lhs->GetType(), type);
    assert_type_match(rhs->GetType(), type);

    return Create<ComparatorInstruction>(
        m_Context.GetInt1Type(),
        m_InsertBlock,
        std::move(name),
        comparator,
        lhs,
        rhs);
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSLT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SLT, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateULT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::ULT, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSGT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SGT, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateUGT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::UGT, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSLE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SLE, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateULE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::ULE, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateSGE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::SGE, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateUGE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::UGE, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateEQ(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::EQU, type, lhs, rhs, std::move(name));
}

scc::ir::ComparatorInstruction *scc::ir::Builder::CreateNE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateComparator(Comparator::NEQ, type, lhs, rhs, std::move(name));
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
    Assert(!!then, "then must not be null");
    Assert(!!else_, "else must not be null");

    assert_type_match(condition->GetType(), m_Context.GetInt1Type());

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

    assert_type_match(value->GetType(), GetInsertFunctionResult());

    return Create<ReturnInstruction>(m_Context.GetVoidType(), m_InsertBlock, value);
}

scc::ir::SelectInstruction *scc::ir::Builder::CreateSelect(
    Type *type,
    std::vector<std::pair<Block *, Value *>> nodes,
    std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!nodes.empty(), "nodes must not be empty");

    for (const auto *value : nodes | std::views::values)
        assert_type_match(value->GetType(), type);

    return Create<SelectInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        std::move(nodes));
}

scc::ir::AllocInstruction *scc::ir::Builder::CreateAlloc(Type *type, uint64_t count, std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!!count, "count must not be 0");

    return Create<AllocInstruction>(
        m_Context.GetPointerType(type),
        m_InsertBlock,
        std::move(name),
        count);
}

scc::ir::LoadInstruction *scc::ir::Builder::CreateLoad(Value *pointer, std::string name)
{
    Assert(!!pointer, "pointer must not be null");

    auto *pointer_type = pointer->GetType();
    Assert(pointer_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", pointer_type);

    auto *element_type = pointer_type->GetElement();

    return Create<LoadInstruction>(
        element_type,
        m_InsertBlock,
        std::move(name),
        pointer);
}

scc::ir::StoreInstruction *scc::ir::Builder::CreateStore(Value *pointer, Value *value)
{
    Assert(!!pointer, "pointer must not be null");
    Assert(!!value, "value must not be null");

    auto *pointer_type = pointer->GetType();
    Assert(pointer_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", pointer_type);

    auto *element_type = pointer_type->GetElement();
    auto *value_type = value->GetType();

    assert_type_match(value_type, element_type);

    return Create<StoreInstruction>(
        m_Context.GetVoidType(),
        m_InsertBlock,
        pointer,
        value);
}

scc::ir::ElementPointerInstruction *scc::ir::Builder::CreateElementPointer(
    Value *pointer,
    std::vector<Value *> indices,
    std::string name)
{
    Assert(!!pointer, "pointer must not be null");
    Assert(!indices.empty(), "indices must not be empty");

    auto *pointer_type = pointer->GetType();

    Assert(pointer_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", pointer_type);

    for (auto *index : indices)
    {
        Assert(!!pointer_type->GetElementCount(), "type {} does not have any elements to index into", pointer_type);

        auto *index_type = index->GetType();
        Assert(index_type->GetKind() == Kind::Int, "type {} is not a kind of int", index_type);

        if (pointer_type->GetKind() == Kind::Struct)
        {
            const auto *constant_index = dynamic_cast<ConstantInt *>(index);

            Assert(!!constant_index, "index must be constant");

            const auto index_value = constant_index->GetValue();

            pointer_type = pointer_type->GetElement(index_value);
        }
        else
        {
            pointer_type = pointer_type->GetElement();
        }
    }

    return Create<ElementPointerInstruction>(
        m_Context.GetPointerType(pointer_type),
        m_InsertBlock,
        std::move(name),
        pointer,
        std::move(indices));
}

scc::ir::ElementPointerInstruction *scc::ir::Builder::CreateElementPointer(
    Value *pointer,
    const std::vector<size_t> &indices,
    std::string name)
{
    Assert(!!pointer, "pointer must not be null");
    Assert(!indices.empty(), "indices must not be empty");

    auto *pointer_type = pointer->GetType();

    Assert(pointer_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", pointer_type);

    std::vector<Value *> values(indices.size());

    for (size_t i = 0; i < indices.size(); ++i)
    {
        const auto index = indices[i];

        pointer_type = pointer_type->GetElement(index);
        values[i] = m_Context.GetInt64(index);
    }

    return Create<ElementPointerInstruction>(
        m_Context.GetPointerType(pointer_type),
        m_InsertBlock,
        std::move(name),
        pointer,
        std::move(values));
}

scc::ir::CallInstruction *scc::ir::Builder::CreateCall(
    FunctionType *function_type,
    Value *callee,
    std::vector<Value *> arguments,
    std::string name)
{
    Assert(!!function_type, "function type must not be null");
    Assert(!!callee, "callee must not be null");
    Assert(!arguments.empty(), "arguments must not be empty");

    auto *callee_type = callee->GetType();

    Assert(callee_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", callee_type);

    auto *callee_element_type = dynamic_cast<PointerType *>(callee_type)->GetElement();

    assert_type_match(callee_element_type, function_type);

    const auto count = function_type->GetArgumentCount();

    Assert(count <= arguments.size(), "not enough arguments");
    Assert(function_type->IsVariadic() || count == arguments.size(), "too many arguments");

    for (size_t i = 0; i < count; ++i)
        assert_type_match(arguments[i]->GetType(), function_type->GetArgument(i));

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

    Assert(type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", type);

    auto *value_type = value->GetType();

    Assert(value_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", value_type);

    return Create<CastInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        value);
}

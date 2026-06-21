#include <scc/ir/block.hpp>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/function.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/module.hpp>
#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

#include <scc/assert.hpp>

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
    Assert(function, "function must not be null");
    Assert(!name.empty(), "name must not be empty");

    if (auto *block = function->FindBlock(name))
        return block;

    return function->CreateBlock(std::move(name));
}

void scc::ir::Builder::SetInsertBlock(Block *block)
{
    Assert(block, "block must not be null");

    m_InsertBlock = block;
}

void scc::ir::Builder::ClearInsertBlock()
{
    m_InsertBlock = {};
}

scc::ir::Block *scc::ir::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

scc::ir::Function *scc::ir::Builder::GetInsertFunction() const
{
    Assert(m_InsertBlock, "insert block must not be null");

    return m_InsertBlock->GetFunction();
}

scc::ir::Type *scc::ir::Builder::GetInsertFunctionResult() const
{
    Assert(m_InsertBlock, "insert block must not be null");

    const auto *function = m_InsertBlock->GetFunction();
    const auto *function_type = dynamic_cast<FunctionType *>(function->GetType()->GetElement());

    return function_type->GetResult();
}

scc::ir::Value *scc::ir::Builder::CreateEmpty(Type *type, std::string name) const
{
    Assert(m_InsertBlock, "insert block must not be null");

    return m_InsertBlock->CreateEmpty(type, std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperator(
    IOperator operator_,
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    Assert(type, "type must not be null");
    Assert(operands.size() >= 2, "at least 2 operands are required");

    Assert(type->GetKind() == Kind::Int, "type {} is not a kind of integer", type);

    for (const auto *operand : operands)
        assert_type_match(operand->GetType(), type);

    return Create<IOperatorInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        operator_,
        std::move(operands));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorADD(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::ADD, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorSUB(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::SUB, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorMUL(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::MUL, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorSDIV(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::SDIV, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorUDIV(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::UDIV, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorSREM(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::SREM, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorUREM(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::UREM, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorAND(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::AND, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorOR(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::OR, type, std::move(operands), std::move(name));
}

scc::ir::IOperatorInstruction *scc::ir::Builder::CreateIOperatorXOR(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateIOperator(IOperator::XOR, type, std::move(operands), std::move(name));
}

scc::ir::FOperatorInstruction *scc::ir::Builder::CreateFOperator(
    FOperator operator_,
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    Assert(type, "type must not be null");
    Assert(operands.size() >= 2, "at least 2 operands are required");

    Assert(type->GetKind() == Kind::Float, "type {} is not a kind of floating point", type);

    for (const auto *operand : operands)
        assert_type_match(operand->GetType(), type);

    return Create<FOperatorInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        operator_,
        std::move(operands));
}

scc::ir::FOperatorInstruction *scc::ir::Builder::CreateFOperatorADD(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateFOperator(FOperator::ADD, type, std::move(operands), std::move(name));
}

scc::ir::FOperatorInstruction *scc::ir::Builder::CreateFOperatorSUB(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateFOperator(FOperator::SUB, type, std::move(operands), std::move(name));
}

scc::ir::FOperatorInstruction *scc::ir::Builder::CreateFOperatorMUL(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateFOperator(FOperator::MUL, type, std::move(operands), std::move(name));
}

scc::ir::FOperatorInstruction *scc::ir::Builder::CreateFOperatorDIV(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateFOperator(FOperator::DIV, type, std::move(operands), std::move(name));
}

scc::ir::FOperatorInstruction *scc::ir::Builder::CreateFOperatorREM(
    Type *type,
    std::vector<Value *> operands,
    std::string name)
{
    return CreateFOperator(FOperator::REM, type, std::move(operands), std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompare(
    ICompare comparator,
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    Assert(type, "type must not be null");
    Assert(lhs, "lhs must not be null");
    Assert(rhs, "rhs must not be null");

    Assert(type->GetKind() == Kind::Int, "type {} is not a kind of integer", type);

    assert_type_match(lhs->GetType(), type);
    assert_type_match(rhs->GetType(), type);

    return Create<ICompareInstruction>(
        m_Context.GetInt1Type(),
        m_InsertBlock,
        std::move(name),
        comparator,
        lhs,
        rhs);
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareSLT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::SLT, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareULT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::ULT, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareSGT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::SGT, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareUGT(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::UGT, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareSLE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::SLE, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareULE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::ULE, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareSGE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::SGE, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareUGE(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::UGE, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareEQU(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::EQU, type, lhs, rhs, std::move(name));
}

scc::ir::ICompareInstruction *scc::ir::Builder::CreateICompareNEQ(
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    return CreateICompare(ICompare::NEQ, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompare(
    FCompare comparator,
    Type *type,
    Value *lhs,
    Value *rhs,
    std::string name)
{
    Assert(type, "type must not be null");
    Assert(lhs, "lhs must not be null");
    Assert(rhs, "rhs must not be null");

    Assert(type->GetKind() == Kind::Float, "type {} is not a kind of floating point", type);

    assert_type_match(lhs->GetType(), type);
    assert_type_match(rhs->GetType(), type);

    return Create<FCompareInstruction>(
        m_Context.GetInt1Type(),
        m_InsertBlock,
        std::move(name),
        comparator,
        lhs,
        rhs);
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareOLT(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::OLT, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareULT(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::ULT, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareOGT(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::OGT, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareUGT(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::UGT, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareOLE(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::OLE, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareULE(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::ULE, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareOGE(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::OGE, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareUGE(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::UGE, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareOEQ(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::OEQ, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareUEQ(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::UEQ, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareONE(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::ONE, type, lhs, rhs, std::move(name));
}

scc::ir::FCompareInstruction *scc::ir::Builder::CreateFCompareUNE(Type *type, Value *lhs, Value *rhs, std::string name)
{
    return CreateFCompare(FCompare::UNE, type, lhs, rhs, std::move(name));
}

scc::ir::DirectBranchInstruction *scc::ir::Builder::CreateBranch(Block *destination)
{
    Assert(destination, "destination must not be null");

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
    Assert(condition, "condition must not be null");
    Assert(then, "then must not be null");
    Assert(else_, "else must not be null");

    assert_type_match(condition->GetType(), m_Context.GetInt1Type());

    return Create<BranchInstruction>(
        m_Context.GetVoidType(),
        m_InsertBlock,
        condition,
        then,
        else_);
}

scc::ir::ReturnInstruction *scc::ir::Builder::CreateReturn()
{
    return Create<ReturnInstruction>(m_Context.GetVoidType(), m_InsertBlock);
}

scc::ir::ReturnInstruction *scc::ir::Builder::CreateReturn(Value *value)
{
    Assert(value, "value must not be null");

    assert_type_match(value->GetType(), GetInsertFunctionResult());

    return Create<ReturnInstruction>(m_Context.GetVoidType(), m_InsertBlock, value);
}

scc::ir::PhiInstruction *scc::ir::Builder::CreatePhi(
    Type *type,
    std::vector<std::pair<Block *, Value *>> nodes,
    std::string name)
{
    Assert(type, "type must not be null");
    Assert(!nodes.empty(), "nodes must not be empty");

    for (const auto *value : nodes | std::views::values)
        assert_type_match(value->GetType(), type);

    return Create<PhiInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        std::move(nodes));
}

scc::ir::AllocInstruction *scc::ir::Builder::CreateAlloc(Type *type, uint64_t count, std::string name)
{
    Assert(type, "type must not be null");
    Assert(count, "count must not be 0");

    return Create<AllocInstruction>(
        m_Context.GetPointerType(type),
        m_InsertBlock,
        std::move(name),
        count);
}

scc::ir::LoadInstruction *scc::ir::Builder::CreateLoad(Value *pointer, std::string name)
{
    Assert(pointer, "pointer must not be null");

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
    Assert(pointer, "pointer must not be null");
    Assert(value, "value must not be null");

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
    Assert(pointer, "pointer must not be null");
    Assert(!indices.empty(), "indices must not be empty");

    auto *pointer_type = pointer->GetType();

    Assert(pointer_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", pointer_type);

    for (auto *index : indices)
    {
        Assert(pointer_type->GetElementCount(), "type {} does not have any elements to index into", pointer_type);

        auto *index_type = index->GetType();
        Assert(index_type->GetKind() == Kind::Int, "type {} is not a kind of int", index_type);

        if (pointer_type->GetKind() == Kind::Struct)
        {
            const auto *constant_index = dynamic_cast<ConstantInt *>(index);

            Assert(constant_index, "index must be constant");

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
    Assert(pointer, "pointer must not be null");
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
    Assert(function_type, "function type must not be null");
    Assert(callee, "callee must not be null");
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
    Assert(type, "type must not be null");
    Assert(value, "value must not be null");

    Assert(type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", type);

    auto *value_type = value->GetType();

    Assert(value_type->GetKind() == Kind::Pointer, "type {} is not a kind of pointer", value_type);

    return Create<CastInstruction>(
        type,
        m_InsertBlock,
        std::move(name),
        value);
}

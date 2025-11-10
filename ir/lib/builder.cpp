#include <ranges>
#include <scc/assert.hpp>
#include <scc/ir/block.hpp>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/module.hpp>
#include <scc/ir/value.hpp>

scc::ir::Builder::Builder(Context &context)
    : m_Context(context)
{
}

scc::ir::ConstantInt::Ptr scc::ir::Builder::CreateI8(uint8_t value) const
{
    auto type = m_Context.GetI8Type();
    return std::make_shared<ConstantInt>(std::move(type), value);
}

scc::ir::ConstantInt::Ptr scc::ir::Builder::CreateI16(uint16_t value) const
{
    auto type = m_Context.GetI16Type();
    return std::make_shared<ConstantInt>(std::move(type), value);
}

scc::ir::ConstantInt::Ptr scc::ir::Builder::CreateI32(uint32_t value) const
{
    auto type = m_Context.GetI32Type();
    return std::make_shared<ConstantInt>(std::move(type), value);
}

scc::ir::ConstantInt::Ptr scc::ir::Builder::CreateI64(uint64_t value) const
{
    auto type = m_Context.GetI64Type();
    return std::make_shared<ConstantInt>(std::move(type), value);
}

scc::ir::ConstantFloat::Ptr scc::ir::Builder::CreateF32(float value) const
{
    auto type = m_Context.GetF32Type();
    return std::make_shared<ConstantFloat>(std::move(type), value);
}

scc::ir::ConstantFloat::Ptr scc::ir::Builder::CreateF64(double value) const
{
    auto type = m_Context.GetF64Type();
    return std::make_shared<ConstantFloat>(std::move(type), value);
}

scc::ir::ConstantArray::Ptr scc::ir::Builder::CreateArray(std::vector<ConstantFwd::Ptr> values) const
{
    Assert(!values.empty(), "values must not be empty");
    auto base = values.front()->GetType();
    for (const auto &value : values)
    {
        Assert(value->GetType() == base, "non-uniform elements");
    }
    auto type = m_Context.GetArrayType(std::move(base), values.size());
    return std::make_shared<ConstantArray>(std::move(type), std::move(values));
}

scc::ir::ConstantVector::Ptr scc::ir::Builder::CreateVector(std::vector<ConstantFwd::Ptr> values) const
{
    Assert(!values.empty(), "values must not be empty");
    auto base = values.front()->GetType();
    for (const auto &value : values)
    {
        Assert(value->GetType() == base, "non-uniform elements");
    }
    auto type = m_Context.GetVectorType(std::move(base), values.size());
    return std::make_shared<ConstantVector>(std::move(type), std::move(values));
}

scc::ir::ConstantStruct::Ptr scc::ir::Builder::CreateStruct(std::vector<ConstantFwd::Ptr> values) const
{
    Assert(!values.empty(), "values must not be empty");
    std::vector<TypeFwd::Ptr> elements;
    for (const auto &value : values)
    {
        elements.emplace_back(value->GetType());
    }
    auto type = m_Context.GetStructType(std::move(elements));
    return std::make_shared<ConstantStruct>(std::move(type), std::move(values));
}

scc::ir::Variable::Ptr scc::ir::Builder::CreateString(
    Module &module,
    std::string name,
    const std::string &value) const
{
    std::vector<ConstantFwd::Ptr> elements;
    for (const auto i : value)
    {
        elements.emplace_back(CreateI8(i));
    }
    elements.emplace_back(CreateI8(0));
    auto type = m_Context.GetArrayType(m_Context.GetI8Type(), elements.size());
    auto initializer = CreateArray(std::move(elements));
    return module.CreateVariable(std::move(type), std::move(name), std::move(initializer));
}

scc::ir::BlockFwd::Ptr scc::ir::Builder::CreateBlock(const Function::Ptr &function, std::string name) const
{
    Assert(!!function, "function must not be null");
    Assert(!name.empty(), "name must not be empty");

    auto block = std::make_shared<Block>(std::move(name), function);
    function->Insert(block);
    return block;
}

void scc::ir::Builder::SetInsertBlock(BlockFwd::Ptr block)
{
    Assert(!!block, "block must not be null");
    m_InsertBlock = std::move(block);
}

void scc::ir::Builder::ClearInsertBlock()
{
    m_InsertBlock = nullptr;
}

scc::ir::BlockFwd::Ptr scc::ir::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateOperator(
    Operator operator_,
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    Assert(!operands.empty(), "operands must not be empty");

    auto type = operands.front()->GetType();
    for (const auto &operand : operands)
    {
        Assert(operand->GetType() == type, "non-uniform operands");
    }

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<OperatorInstruction>(
        std::move(type),
        std::move(register_),
        m_InsertBlock,
        operator_,
        std::move(operands));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateAdd(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_Add, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateSub(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_Sub, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateMul(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_Mul, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateDiv(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_Div, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateRem(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_Rem, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateAnd(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_And, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateOr(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_Or, std::move(operands), std::move(name));
}

scc::ir::OperatorInstruction::Ptr scc::ir::Builder::CreateXor(
    std::vector<ValueFwd::Ptr> operands,
    std::string name)
{
    return CreateOperator(Operator_Xor, std::move(operands), std::move(name));
}

scc::ir::ComparatorInstruction::Ptr scc::ir::Builder::CreateComparator(
    Comparator comparator,
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs,
    std::string name)
{
    Assert(!!lhs, "lhs must not be null");
    Assert(!!rhs, "rhs must not be null");
    Assert(lhs->GetType() == rhs->GetType(), "rhs type must be same as lhs type");

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<ComparatorInstruction>(
        m_Context.GetI1Type(),
        std::move(register_),
        m_InsertBlock,
        comparator,
        std::move(lhs),
        std::move(rhs));
}

scc::ir::ComparatorInstruction::Ptr scc::ir::Builder::CreateLT(
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs,
    std::string name)
{
    return CreateComparator(Comparator_LT, std::move(lhs), std::move(rhs), std::move(name));
}

scc::ir::ComparatorInstruction::Ptr scc::ir::Builder::CreateGT(
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs,
    std::string name)
{
    return CreateComparator(Comparator_GT, std::move(lhs), std::move(rhs), std::move(name));
}

scc::ir::ComparatorInstruction::Ptr scc::ir::Builder::CreateLE(
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs,
    std::string name)
{
    return CreateComparator(Comparator_LE, std::move(lhs), std::move(rhs), std::move(name));
}

scc::ir::ComparatorInstruction::Ptr scc::ir::Builder::CreateGE(
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs,
    std::string name)
{
    return CreateComparator(Comparator_GE, std::move(lhs), std::move(rhs), std::move(name));
}

scc::ir::ComparatorInstruction::Ptr scc::ir::Builder::CreateEQ(
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs,
    std::string name)
{
    return CreateComparator(Comparator_EQ, std::move(lhs), std::move(rhs), std::move(name));
}

scc::ir::ComparatorInstruction::Ptr scc::ir::Builder::CreateNE(
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs,
    std::string name)
{
    return CreateComparator(Comparator_NE, std::move(lhs), std::move(rhs), std::move(name));
}

scc::ir::BranchInstruction::Ptr scc::ir::Builder::CreateBranch(BlockFwd::Ptr destination)
{
    Assert(!!destination, "destination must not be null");

    auto register_ = m_InsertBlock->CreateRegister();

    return Create<BranchInstruction>(
        m_Context.GetVoidType(),
        std::move(register_),
        m_InsertBlock,
        std::move(destination));
}

scc::ir::BranchInstruction::Ptr scc::ir::Builder::CreateBranch(
    ValueFwd::Ptr condition,
    BlockFwd::Ptr then,
    BlockFwd::Ptr else_)
{
    Assert(!!condition, "condition must not be null");
    Assert(condition->GetType() == m_Context.GetI1Type(), "condition type must be i1");
    Assert(!!then, "then must not be null");
    Assert(!!else_, "else must not be null");

    auto register_ = m_InsertBlock->CreateRegister();

    return Create<BranchInstruction>(
        m_Context.GetVoidType(),
        std::move(register_),
        m_InsertBlock,
        std::move(condition),
        std::move(then),
        std::move(else_));
}

scc::ir::ReturnInstruction::Ptr scc::ir::Builder::CreateRet()
{
    auto register_ = m_InsertBlock->CreateRegister();

    return Create<ReturnInstruction>(m_Context.GetVoidType(), std::move(register_), m_InsertBlock, nullptr);
}

scc::ir::ReturnInstruction::Ptr scc::ir::Builder::CreateRet(ValueFwd::Ptr value)
{
    Assert(!!value, "value must not be null");

    auto register_ = m_InsertBlock->CreateRegister();

    return Create<ReturnInstruction>(m_Context.GetVoidType(), std::move(register_), m_InsertBlock, std::move(value));
}

scc::ir::SelectInstruction::Ptr scc::ir::Builder::CreateSelect(
    std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> options,
    std::string name)
{
    Assert(!options.empty(), "options must not be empty");

    auto type = options.front().second->GetType();
    for (const auto &value : options | std::views::values)
    {
        Assert(value->GetType() == type, "non-uniform options");
    }

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<SelectInstruction>(
        std::move(type),
        std::move(register_),
        m_InsertBlock,
        std::move(options));
}

scc::ir::AllocInstruction::Ptr scc::ir::Builder::CreateAlloc(TypeFwd::Ptr type, std::string name, unsigned count)
{
    Assert(!!type, "type must not be null");

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<AllocInstruction>(
        m_Context.GetPointerType(std::move(type)),
        std::move(register_),
        m_InsertBlock,
        count);
}

scc::ir::LoadInstruction::Ptr scc::ir::Builder::CreateLoad(ValueFwd::Ptr pointer, std::string name)
{
    Assert(!!pointer, "pointer must not be null");
    Assert(pointer->GetType()->GetKind() == Kind_Pointer, "pointer type must be a kind of pointer");

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    auto type = std::dynamic_pointer_cast<PointerType>(pointer->GetType())->GetBase();
    return Create<LoadInstruction>(
        std::move(type),
        std::move(register_),
        m_InsertBlock,
        std::move(pointer));
}

scc::ir::StoreInstruction::Ptr scc::ir::Builder::CreateStore(ValueFwd::Ptr pointer, ValueFwd::Ptr value)
{
    Assert(!!pointer, "pointer must not be null");
    Assert(!!value, "value must not be null");
    Assert(pointer->GetType()->GetKind() == Kind_Pointer, "pointer type must be a kind of pointer");

    const auto type = std::dynamic_pointer_cast<PointerType>(pointer->GetType())->GetBase();
    Assert(value->GetType() == type, "value type must be the same as pointer base type");

    auto register_ = m_InsertBlock->CreateRegister();

    return Create<StoreInstruction>(
        m_Context.GetVoidType(),
        std::move(register_),
        m_InsertBlock,
        std::move(pointer),
        std::move(value));
}

scc::ir::OffsetInstruction::Ptr scc::ir::Builder::CreateOffset(
    TypeFwd::Ptr type,
    ValueFwd::Ptr base,
    std::vector<ValueFwd::Ptr> offsets,
    std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!!base, "base must not be null");
    Assert(!offsets.empty(), "offsets must not be empty");

    Assert(base->GetType()->GetKind() == Kind_Pointer, "base type must be a kind of pointer");

    for (const auto &offset : offsets)
    {
        Assert(offset->GetType()->GetKind() == Kind_Int, "offset type must be a kind of int");
    }

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<OffsetInstruction>(
        std::move(type),
        std::move(register_),
        m_InsertBlock,
        std::move(base),
        std::move(offsets));
}

scc::ir::OffsetInstruction::Ptr scc::ir::Builder::CreateConstOffset(
    ValueFwd::Ptr base,
    const std::vector<unsigned> &offsets,
    std::string name)
{
    std::vector<ValueFwd::Ptr> values;

    auto type = base->GetType();
    for (const auto offset : offsets)
    {
        Assert(offset < type->GetElementCount(), "constant offset out of bounds");
        type = type->GetElement(offset);

        values.emplace_back(CreateI64(offset));
    }

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<OffsetInstruction>(
        m_Context.GetPointerType(std::move(type)),
        std::move(register_),
        m_InsertBlock,
        std::move(base),
        std::move(values));
}

scc::ir::CallInstruction::Ptr scc::ir::Builder::CreateCall(
    ValueFwd::Ptr callee,
    std::vector<ValueFwd::Ptr> arguments,
    std::string name)
{
    Assert(!!callee, "callee must not be null");
    Assert(!arguments.empty(), "arguments must not be empty");

    Assert(callee->GetType()->GetKind() == Kind_Function, "callee type must be a kind of function");

    const auto function_type = std::dynamic_pointer_cast<FunctionType>(callee->GetType());

    const auto count = function_type->GetArgumentCount();
    Assert(count <= arguments.size(), "not enough arguments");
    Assert(function_type->IsVariadic() || count == arguments.size(), "too many arguments");

    for (unsigned i = 0; i < count; ++i)
    {
        Assert(function_type->GetArgument(i) == arguments.at(i)->GetType(), "invalid argument type");
    }

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<CallInstruction>(
        function_type->GetResult(),
        std::move(register_),
        m_InsertBlock,
        std::move(callee),
        std::move(arguments));
}

scc::ir::CastInstruction::Ptr scc::ir::Builder::CreateCast(
    TypeFwd::Ptr type,
    ValueFwd::Ptr value,
    std::string name)
{
    Assert(!!type, "type must not be null");
    Assert(!!value, "value must not be null");

    auto register_ = m_InsertBlock->CreateRegister(std::move(name));

    return Create<CastInstruction>(
        std::move(type),
        std::move(register_),
        m_InsertBlock,
        std::move(value));
}

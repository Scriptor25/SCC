#include <scc/assert.hpp>
#include <scc/ir/block.hpp>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>

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

scc::ir::ConstantArray::Ptr scc::ir::Builder::CreateArray(std::vector<ConstantPtr> values) const
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

scc::ir::ConstantVector::Ptr scc::ir::Builder::CreateVector(std::vector<ConstantPtr> values) const
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

scc::ir::ConstantStruct::Ptr scc::ir::Builder::CreateStruct(std::vector<ConstantPtr> values) const
{
    Assert(!values.empty(), "values must not be empty");
    std::vector<TypePtr> elements;
    for (const auto &value : values)
    {
        elements.emplace_back(value->GetType());
    }
    auto type = m_Context.GetStructType(std::move(elements));
    return std::make_shared<ConstantStruct>(std::move(type), std::move(values));
}

scc::ir::BlockPtr scc::ir::Builder::CreateBlock(const Function::Ptr &function, std::string name) const
{
    Assert(!!function, "function must not be null");
    Assert(!name.empty(), "name must not be empty");

    auto block = std::make_shared<Block>(std::move(name), function);
    function->Insert(block);
    return block;
}

void scc::ir::Builder::SetInsertBlock(BlockPtr block)
{
    m_InsertBlock = std::move(block);
}

scc::ir::BlockPtr scc::ir::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

scc::ir::Shared<scc::ir::OperatorInstruction>::Ptr scc::ir::Builder::CreateOperator(
    TypePtr type,
    std::string name,
    Operator operator_,
    std::vector<ValuePtr> operands)
{
    Assert(!!type, "type must not be null");
    Assert(!name.empty(), "name must not be empty");
    Assert(!operands.empty(), "operands must not be empty");

    auto instruction = std::make_shared<OperatorInstruction>(
        std::move(type),
        std::move(name),
        m_InsertBlock,
        operator_,
        std::move(operands));
    if (m_InsertBlock)
    {
        m_InsertBlock->Insert(instruction);
    }
    return instruction;
}

scc::ir::Shared<scc::ir::ComparatorInstruction>::Ptr scc::ir::Builder::CreateComparator(
    TypePtr type,
    std::string name,
    Comparator comparator,
    ValuePtr lhs,
    ValuePtr rhs)
{
    Assert(!!type, "type must not be null");
    Assert(!name.empty(), "name must not be empty");
    Assert(!!lhs, "lhs must not be null");
    Assert(!!rhs, "rhs must not be null");

    auto instruction = std::make_shared<ComparatorInstruction>(
        std::move(type),
        std::move(name),
        m_InsertBlock,
        comparator,
        std::move(lhs),
        std::move(rhs));
    if (m_InsertBlock)
    {
        m_InsertBlock->Insert(instruction);
    }
    return instruction;
}

scc::ir::Shared<scc::ir::BranchInstruction>::Ptr scc::ir::Builder::CreateBranch(BlockPtr destination)
{
    Assert(!!destination, "destination must not be null");

    auto instruction = std::make_shared<BranchInstruction>(m_InsertBlock, std::move(destination));
    if (m_InsertBlock)
    {
        m_InsertBlock->Insert(instruction);
    }
    return instruction;
}

scc::ir::Shared<scc::ir::BranchInstruction>::Ptr scc::ir::Builder::CreateBranch(
    ValuePtr condition,
    BlockPtr then,
    BlockPtr else_)
{
    Assert(!!condition, "condition must not be null");
    Assert(!!then, "then must not be null");
    Assert(!!else_, "else must not be null");

    auto instruction = std::make_shared<BranchInstruction>(
        m_InsertBlock,
        std::move(condition),
        std::move(then),
        std::move(else_));
    if (m_InsertBlock)
    {
        m_InsertBlock->Insert(instruction);
    }
    return instruction;
}

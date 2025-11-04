#include <scc/assert.hpp>
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
    Assert(!values.empty(), "elements must not be empty");
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
    Assert(!values.empty(), "elements must not be empty");
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
    Assert(!values.empty(), "elements must not be empty");
    std::vector<TypePtr> elements;
    for (const auto &value : values)
    {
        elements.emplace_back(value->GetType());
    }
    auto type = m_Context.GetStructType(std::move(elements));
    return std::make_shared<ConstantStruct>(std::move(type), std::move(values));
}

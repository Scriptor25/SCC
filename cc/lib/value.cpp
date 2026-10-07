#include <scc/cc/builder.hpp>
#include <scc/cc/value.hpp>

#include <scc/ir/builder.hpp>

#include <scc/assert.hpp>

std::unique_ptr<scc::cc::RValue> scc::cc::Value::CreateR(ir::Value *value)
{
    return std::make_unique<RValue>(value);
}

std::unique_ptr<scc::cc::LValue> scc::cc::Value::CreateL(ir::Value *pointer)
{
    return std::make_unique<LValue>(pointer);
}

scc::cc::RValue::RValue(ir::Value *value)
    : m_Value(value)
{
}

scc::ir::Value *scc::cc::RValue::Load(Builder &builder) const
{
    return m_Value;
}

void scc::cc::RValue::Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const
{
    Error("rvalue is immutable");
}

scc::cc::LValue::LValue(ir::Value *pointer)
    : m_Pointer(pointer)
{
}

scc::ir::Value *scc::cc::LValue::Load(Builder &builder) const
{
    return builder.GetBuilder().CreateLoad(m_Pointer);
}

void scc::cc::LValue::Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const
{
    builder.CreateStore(m_Pointer, value);
}

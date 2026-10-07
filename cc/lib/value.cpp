#include <scc/cc/builder.hpp>
#include <scc/cc/value.hpp>

#include <scc/ir/builder.hpp>

#include <scc/assert.hpp>

std::unique_ptr<scc::cc::RValue> scc::cc::Value::CreateR(Type *type, ir::Value *value)
{
    return std::make_unique<RValue>(type, value);
}

std::unique_ptr<scc::cc::LValue> scc::cc::Value::CreateL(Type *type, ir::Value *pointer)
{
    return std::make_unique<LValue>(type, pointer);
}

scc::cc::Value::Value(Type *type)
    : m_Type(type)
{
}

scc::cc::Type *scc::cc::Value::GetType() const
{
    return m_Type;
}

scc::cc::RValue::RValue(Type *type, ir::Value *value)
    : Value(type),
      m_Value(value)
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

scc::cc::LValue::LValue(Type *type, ir::Value *pointer)
    : Value(type),
      m_Pointer(pointer)
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

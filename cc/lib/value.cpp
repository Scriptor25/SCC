#include <scc/cc/builder.hpp>
#include <scc/cc/value.hpp>

#include <scc/ir/builder.hpp>

#include <scc/assert.hpp>

std::unique_ptr<scc::cc::RValue> scc::cc::Value::CreateR(const Type *type, ir::Value *value)
{
    return std::make_unique<RValue>(type, value);
}

std::unique_ptr<scc::cc::LValue> scc::cc::Value::CreateL(const Type *type, ir::Value *pointer)
{
    return std::make_unique<LValue>(type, pointer);
}

scc::cc::Value::Value(const Type *type)
    : m_Type(type)
{
}

const scc::cc::Type *scc::cc::Value::GetType() const
{
    return m_Type;
}

scc::cc::RValue::RValue(const Type *type, ir::Value *value)
    : Value(type),
      m_Value(value)
{
}

scc::ir::Value *scc::cc::RValue::Load(Builder &, bool) const
{
    return m_Value;
}

void scc::cc::RValue::Store(ir::Context &, ir::Builder &, ir::Value *, bool) const
{
    Error("rvalue is immutable");
}

scc::cc::LValue::LValue(const Type *type, ir::Value *pointer)
    : Value(type),
      m_Pointer(pointer)
{
}

scc::ir::Value *scc::cc::LValue::Load(Builder &builder, const bool is_volatile) const
{
    return builder.GetIRBuilder().CreateLoad(m_Pointer, is_volatile);
}

void scc::cc::LValue::Store(ir::Context &, ir::Builder &builder, ir::Value *value, const bool is_volatile) const
{
    builder.CreateStore(m_Pointer, value, is_volatile);
}

#include <scc/cc/builder.hpp>
#include <scc/cc/value.hpp>

#include <scc/ir/builder.hpp>

#include <scc/assert.hpp>

std::unique_ptr<scc::cc::RValue> scc::cc::Value::CreateR(QualifiedType type, ir::Value *value)
{
    return std::make_unique<RValue>(type, value);
}

std::unique_ptr<scc::cc::LValue> scc::cc::Value::CreateL(QualifiedType type, ir::Value *pointer)
{
    return std::make_unique<LValue>(type, pointer);
}

scc::cc::Value::Value(const QualifiedType type)
    : Ty(type)
{
}

scc::cc::RValue::RValue(const QualifiedType type, ir::Value *val)
    : Value(type),
      Val(val)
{
}

scc::ir::Value *scc::cc::RValue::Load(Builder &, bool) const
{
    return Val;
}

void scc::cc::RValue::Store(ir::Context &, ir::Builder &, ir::Value *, bool) const
{
    Error("rvalue");
}

scc::ir::Value *scc::cc::RValue::Reference() const
{
    Error("rvalue");
}

scc::cc::LValue::LValue(const QualifiedType type, ir::Value *ptr)
    : Value(type),
      Ptr(ptr)
{
}

scc::ir::Value *scc::cc::LValue::Load(Builder &builder, const bool is_volatile) const
{
    return builder.GetIRBuilder().CreateLoad(Ptr, is_volatile);
}

void scc::cc::LValue::Store(ir::Context &, ir::Builder &builder, ir::Value *value, const bool is_volatile) const
{
    builder.CreateStore(Ptr, value, is_volatile);
}

scc::ir::Value *scc::cc::LValue::Reference() const
{
    return Ptr;
}

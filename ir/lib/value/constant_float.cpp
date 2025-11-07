#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantFloat::ConstantFloat(FloatType::Ptr type, const double value)
    : Constant(std::move(type)),
      m_Value(value)
{
}

std::ostream &scc::ir::ConstantFloat::PrintOperand(std::ostream &stream) const
{
    return m_Type->Print(stream) << " 0x" << std::hexfloat << m_Value << std::defaultfloat;
}

double scc::ir::ConstantFloat::GetValue() const
{
    return m_Value;
}

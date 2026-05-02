#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

scc::ir::ConstantFloat::ConstantFloat(FloatType *type, const float64_t value)
    : Constant(type),
      m_Value(value)
{
}

std::ostream &scc::ir::ConstantFloat::PrintOperand(std::ostream &stream) const
{
    return m_Type->Print(stream) << " 0x" << std::hexfloat << m_Value << std::defaultfloat;
}

bool scc::ir::ConstantFloat::Compare(Constant *value) const
{
    if (const auto p = dynamic_cast<ConstantFloat *>(value))
        return m_Type == p->m_Type && m_Value == p->m_Value;

    return false;
}

scc::ir::float64_t scc::ir::ConstantFloat::GetValue() const
{
    return m_Value;
}

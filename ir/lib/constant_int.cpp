#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

scc::ir::ConstantInt::ConstantInt(IntType *type, const uint64_t value)
    : Constant(type),
      m_Value(value)
{
}

std::ostream &scc::ir::ConstantInt::PrintOperand(std::ostream &stream) const
{
    return m_Type->Print(stream) << " 0x" << std::hex << m_Value << std::dec;
}

bool scc::ir::ConstantInt::Compare(Constant *value) const
{
    if (const auto p = dynamic_cast<ConstantInt *>(value))
        return m_Type == p->m_Type && m_Value == p->m_Value;

    return false;
}

uint64_t scc::ir::ConstantInt::GetValue() const
{
    return m_Value;
}

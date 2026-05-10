#include <scc/as/operand.hpp>

#include <ostream>

scc::as::ReferenceOperand::ReferenceOperand(
    const Immediate displacement,
    const Register base_register,
    const Register index_register,
    const Immediate scale)
    : m_Displacement(displacement),
      m_BaseRegister(base_register),
      m_IndexRegister(index_register),
      m_Scale(scale)
{
}

std::ostream &scc::as::ReferenceOperand::Print(std::ostream &stream) const
{
    if (m_Displacement)
        stream << "0x" << std::hex << m_Displacement;

    stream << "(%" << std::dec << m_BaseRegister;

    if (m_IndexRegister)
        stream << ", %" << std::dec << m_IndexRegister;

    if (m_Scale)
        stream << ", 0x" << std::hex << m_Scale;

    return stream << ')';
}

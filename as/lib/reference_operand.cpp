#include <scc/as/operand.hpp>

#include <ostream>
#include <scc/platform.hpp>

scc::as::ReferenceOperand::ReferenceOperand(
    const Platform &platform,
    const Immediate displacement,
    const Register base_register,
    const Register index_register,
    const Immediate scale)
    : Operand(platform),
      m_Displacement(displacement),
      m_BaseRegister(base_register),
      m_IndexRegister(index_register),
      m_Scale(scale)
{
}

std::ostream &scc::as::ReferenceOperand::Print(std::ostream &stream) const
{
    if (m_Displacement)
        stream << "0x" << std::hex << m_Displacement;

    stream << "(%" << m_Platform.ISA.RegisterViews.at(m_BaseRegister).Name;

    if (m_IndexRegister != Register::None)
        stream << ", %" << m_Platform.ISA.RegisterViews.at(m_IndexRegister).Name;

    if (m_Scale)
        stream << ", 0x" << std::hex << m_Scale;

    return stream << ')';
}

#include <scc/platform.hpp>
#include <scc/as/operand.hpp>

#include <format>
#include <ostream>

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
        stream << std::format("0x{:X}", m_Displacement);

    stream << "(%" << m_Platform.ISA.RegisterViews.at(m_BaseRegister).Name;

    if (m_IndexRegister != Register::None)
        stream << ", %" << m_Platform.ISA.RegisterViews.at(m_IndexRegister).Name;

    if (m_Scale)
        stream << std::format(", 0x{:X}", m_Scale);

    return stream << ')';
}

scc::as::Immediate scc::as::ReferenceOperand::GetDisplacement() const
{
    return m_Displacement;
}

scc::Register scc::as::ReferenceOperand::GetBaseRegister() const
{
    return m_BaseRegister;
}

scc::Register scc::as::ReferenceOperand::GetIndexRegister() const
{
    return m_IndexRegister;
}

scc::as::Immediate scc::as::ReferenceOperand::GetScale() const
{
    return m_Scale;
}

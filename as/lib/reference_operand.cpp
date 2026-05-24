#include <scc/as/operand.hpp>

#include <scc/platform.hpp>

#include <ostream>

scc::as::ReferenceOperand::ReferenceOperand(const Platform &platform, int64_t displacement)
    : Operand(platform),
      m_Displacement(displacement),
      m_HasBaseRegister(false),
      m_BaseRegister(),
      m_HasIndexRegister(false),
      m_IndexRegister(),
      m_Scale()
{
}

scc::as::ReferenceOperand::ReferenceOperand(
    const Platform &platform,
    const int64_t displacement,
    const bool has_base_register,
    const Register base_register,
    const bool has_index_register,
    const Register index_register,
    const uint8_t scale)
    : Operand(platform),
      m_Displacement(displacement),
      m_HasBaseRegister(has_base_register),
      m_BaseRegister(base_register),
      m_HasIndexRegister(has_index_register),
      m_IndexRegister(index_register),
      m_Scale(scale)
{
}

std::ostream &scc::as::ReferenceOperand::Print(std::ostream &stream) const
{
    if (!m_HasBaseRegister || m_Displacement)
        stream << "0x" << std::hex << m_Displacement;

    if (!m_HasBaseRegister)
        return stream;

    stream << "(%" << m_Platform.ISA.FindRegisterView(m_BaseRegister)->CanonicalName();

    if (m_HasIndexRegister)
    {
        stream << ", %" << m_Platform.ISA.FindRegisterView(m_IndexRegister)->CanonicalName();

        if (m_Scale)
            stream << ", 0x" << std::hex << m_Scale;
    }

    return stream << ')';
}

int64_t scc::as::ReferenceOperand::GetDisplacement() const
{
    return m_Displacement;
}

bool scc::as::ReferenceOperand::HasBaseRegister() const
{
    return m_HasBaseRegister;
}

scc::Register scc::as::ReferenceOperand::GetBaseRegister() const
{
    return m_BaseRegister;
}

bool scc::as::ReferenceOperand::HasIndexRegister() const
{
    return m_HasIndexRegister;
}

scc::Register scc::as::ReferenceOperand::GetIndexRegister() const
{
    return m_IndexRegister;
}

uint8_t scc::as::ReferenceOperand::GetScale() const
{
    return m_Scale;
}

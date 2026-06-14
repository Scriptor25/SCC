#include <scc/as/operand.hpp>

#include <scc/platform.hpp>

#include <ostream>

scc::as::MemoryOperand::MemoryOperand(
    const Platform &platform,
    const int64_t displacement)
    : Operand(platform),
      m_Displacement(displacement),
      m_Scale()
{
}

scc::as::MemoryOperand::MemoryOperand(
    const Platform &platform,
    const int64_t displacement,
    const std::optional<Register> base_register,
    const std::optional<Register> index_register,
    const uint8_t scale)
    : Operand(platform),
      m_Displacement(displacement),
      m_BaseRegister(base_register),
      m_IndexRegister(index_register),
      m_Scale(scale)
{
}

std::ostream &scc::as::MemoryOperand::Print(std::ostream &stream) const
{
    PrintSelector(stream);

    if (!m_BaseRegister || m_Displacement)
        stream << std::dec << m_Displacement;

    if (!m_BaseRegister)
        return stream;

    stream << "(%" << m_Platform.ISA.FindRegisterView(*m_BaseRegister)->CanonicalName();

    if (m_IndexRegister)
    {
        stream << ", %" << m_Platform.ISA.FindRegisterView(*m_IndexRegister)->CanonicalName();

        if (m_Scale)
            stream << ", " << std::dec << m_Scale;
    }

    return stream << ')';
}

int64_t scc::as::MemoryOperand::GetDisplacement() const
{
    return m_Displacement;
}

bool scc::as::MemoryOperand::HasBaseRegister() const
{
    return !!m_BaseRegister;
}

scc::Register scc::as::MemoryOperand::GetBaseRegister() const
{
    return *m_BaseRegister;
}

bool scc::as::MemoryOperand::HasIndexRegister() const
{
    return !!m_IndexRegister;
}

scc::Register scc::as::MemoryOperand::GetIndexRegister() const
{
    return *m_IndexRegister;
}

uint8_t scc::as::MemoryOperand::GetScale() const
{
    return m_Scale;
}

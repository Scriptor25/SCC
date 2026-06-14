#include <iomanip>
#include <scc/as/operand.hpp>

#include <scc/assert.hpp>
#include <scc/platform.hpp>

scc::as::Operand::Operand(const Platform &platform)
    : m_Platform(platform),
      m_SegmentSelector()
{
}

const scc::Platform &scc::as::Operand::GetPlatform() const
{
    return m_Platform;
}

scc::as::Immediate scc::as::Operand::GetImmediate() const
{
    Error("operand is not immediate");
}

std::ostream &scc::as::Operand::PrintSelector(std::ostream &stream) const
{
    if (m_SegmentRegister)
        return stream << '%' << m_Platform.ISA.FindRegisterView(*m_SegmentRegister)->CanonicalName() << ':';

    if (m_SegmentSelector)
        return stream
               << "0x"
               << std::hex
               << std::setfill('0')
               << std::setw(2)
               << static_cast<unsigned>(m_SegmentSelector)
               << ':';

    return stream;
}

uint8_t scc::as::Operand::GetSegmentSelector() const
{
    return m_SegmentSelector;
}

bool scc::as::Operand::HasSegmentRegister() const
{
    return !!m_SegmentRegister;
}

scc::Register scc::as::Operand::GetSegmentRegister() const
{
    return *m_SegmentRegister;
}

void scc::as::Operand::SetSegmentSelector(const uint8_t sel)
{
    m_SegmentSelector = sel;
}

void scc::as::Operand::SetSegmentRegister(const Register reg)
{
    m_SegmentRegister = reg;
}

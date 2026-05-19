#include <scc/as/operand.hpp>

#include <format>

scc::as::DirectOperand::DirectOperand(const Platform &platform, const Immediate address)
    : Operand(platform),
      m_Address(address)
{
}

std::ostream &scc::as::DirectOperand::Print(std::ostream &stream) const
{
    return stream << std::format("0x{:X}", m_Address);
}

scc::as::Immediate scc::as::DirectOperand::GetAddress() const
{
    return m_Address;
}

#include <scc/as/operand.hpp>

#include <ostream>

scc::as::ImmediateOperand::ImmediateOperand(const Immediate immediate)
    : m_Immediate(immediate)
{
}

std::ostream &scc::as::ImmediateOperand::Print(std::ostream &stream) const
{
    return stream << "0x" << std::hex << m_Immediate;
}

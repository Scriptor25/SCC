#include <scc/as/operand.hpp>

#include <format>

scc::as::ImmediateOperand::ImmediateOperand(const Platform &platform, const Immediate value)
    : Operand(platform),
      m_Value(value)
{
}

scc::as::Immediate scc::as::ImmediateOperand::GetImmediate() const
{
    return m_Value;
}

std::ostream &scc::as::ImmediateOperand::Print(std::ostream &stream) const
{
    return stream << std::format("$0x{:X}", m_Value);
}

scc::as::Immediate scc::as::ImmediateOperand::GetValue() const
{
    return m_Value;
}

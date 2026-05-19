#include <scc/as/operand.hpp>

#include <ostream>

scc::as::StringOperand::StringOperand(const Platform &platform, std::string value)
    : Operand(platform),
      m_Value(std::move(value))
{
}

std::ostream &scc::as::StringOperand::Print(std::ostream &stream) const
{
    return stream << '"' << m_Value << '"';
}

const std::string &scc::as::StringOperand::GetValue() const
{
    return m_Value;
}

#include <scc/as/operand.hpp>

#include <ostream>

scc::as::RegisterOperand::RegisterOperand(const Register register_)
    : m_Register(register_)
{
}

std::ostream &scc::as::RegisterOperand::Print(std::ostream &stream) const
{
    return stream << '%' << std::dec << m_Register;
}

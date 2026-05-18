#include <scc/as/operand.hpp>

#include <scc/platform.hpp>

#include <ostream>

scc::as::RegisterOperand::RegisterOperand(const Platform &platform, const Register register_)
    : Operand(platform),
      m_Register(register_)
{
}

std::ostream &scc::as::RegisterOperand::Print(std::ostream &stream) const
{
    return stream << '%' << m_Platform.ISA.RegisterViews.at(m_Register).Name;
}

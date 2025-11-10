#include <iostream>
#include <scc/ir/register.hpp>

scc::ir::Register::Register(std::string name)
    : m_Name(std::move(name))
{
}

std::string scc::ir::Register::GetName() const
{
    return m_Name;
}

void scc::ir::Register::SetName(std::string name)
{
    m_Name = std::move(name);
}

std::ostream &scc::ir::Register::Print(std::ostream &stream) const
{
    return stream << '%' << m_Name;
}

#include <scc/ir/value.hpp>

scc::ir::Global::Global(TypeFwd::Ptr type, std::string name)
    : Value(std::move(type)),
      m_Name(std::move(name))
{
}

std::ostream &scc::ir::Global::PrintOperand(std::ostream &stream) const
{
    return stream << '@' << m_Name;
}

std::string scc::ir::Global::GetName() const
{
    return m_Name;
}

void scc::ir::Global::SetName(std::string name)
{
    m_Name = std::move(name);
}

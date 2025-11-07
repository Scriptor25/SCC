#include <scc/ir/value.hpp>

scc::ir::NamedValue::NamedValue(TypePtr type, std::string name)
    : Value(std::move(type)),
      m_Name(std::move(name))
{
}

std::string scc::ir::NamedValue::GetName() const
{
    return m_Name;
}

void scc::ir::NamedValue::SetName(std::string name)
{
    m_Name = std::move(name);
}

#include <iostream>
#include <scc/assert.hpp>
#include <scc/ir/register.hpp>

scc::ir::Register::Register(std::string name)
    : m_Name(std::move(name))
{
}

void scc::ir::Register::SetValue(ValueFwd::WeakPtr value)
{
    m_Value = std::move(value);
}

void scc::ir::Register::ClearValue()
{
    m_Value.reset();
}

std::string scc::ir::Register::GetName() const
{
    return m_Name;
}

void scc::ir::Register::SetName(std::string name)
{
    Assert(!name.empty(), "name must not be empty");
    m_Name = std::move(name);
}

scc::ir::Shared<scc::ir::Value>::Ptr scc::ir::Register::GetValue() const
{
    return m_Value.lock();
}

std::ostream &scc::ir::Register::Print(std::ostream &stream) const
{
    return stream << '%' << m_Name;
}

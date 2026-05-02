#include <scc/ir/argument.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

#include <ostream>

scc::ir::Argument::Argument(Type *type)
    : Value(type)
{
}

scc::ir::Argument::Argument(Type *type, std::string name)
    : Value(type),
      m_Name(std::move(name))
{
}

std::ostream &scc::ir::Argument::Print(std::ostream &stream) const
{
    if (m_Name.empty())
        return m_Type->Print(stream);

    return m_Type->Print(stream) << " %" << m_Name;
}

std::ostream &scc::ir::Argument::PrintOperand(std::ostream &stream) const
{
    Assert(!m_Name.empty(), "cannot use unnamed argument value");

    return stream << '%' << m_Name;
}

void scc::ir::Argument::SetName(std::string name)
{
    m_Name = std::move(name);
}

const std::string &scc::ir::Argument::GetName() const
{
    return m_Name;
}

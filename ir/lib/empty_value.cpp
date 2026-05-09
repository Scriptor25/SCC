#include <scc/ir/empty_value.hpp>

#include <scc/assert.hpp>
#include <scc/ir/type.hpp>

scc::ir::EmptyValue::EmptyValue(Type *type, std::string name)
    : Value(type),
      m_Name(std::move(name))
{
}

std::ostream &scc::ir::EmptyValue::Print(std::ostream &stream) const
{
    if (m_Name.empty())
        return stream << "<empty>";

    return m_Type->Print(stream << '%' << m_Name << " = ") << " <empty>";
}

std::ostream &scc::ir::EmptyValue::PrintOperand(std::ostream &stream, bool print_type) const
{
    Assert(!m_Name.empty(), "cannot use nameless value");

    if (print_type)
        m_Type->Print(stream) << ' ';

    return stream << '%' << m_Name;
}

void scc::ir::EmptyValue::SetName(std::string name)
{
    m_Name = std::move(name);
}

const std::string &scc::ir::EmptyValue::GetName() const
{
    return m_Name;
}

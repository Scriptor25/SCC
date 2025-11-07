#include <scc/ir/value.hpp>

scc::ir::Argument::Argument(TypePtr type, std::string name)
    : NamedValue(std::move(type), std::move(name))
{
}

std::ostream &scc::ir::Argument::PrintOperand(std::ostream &stream) const
{
    return stream << '%' << m_Name;
}

std::ostream &scc::ir::Argument::Print(std::ostream &stream) const
{
    if (m_Name.empty())
    {
        return m_Type->Print(stream);
    }

    return m_Type->Print(stream) << " %" << m_Name;
}

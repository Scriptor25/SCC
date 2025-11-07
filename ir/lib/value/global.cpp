#include <scc/ir/value.hpp>

scc::ir::Global::Global(TypePtr type, std::string name)
    : NamedValue(std::move(type), std::move(name))
{
}

std::ostream &scc::ir::Global::PrintOperand(std::ostream &stream) const
{
    return stream << '@' << m_Name;
}

#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::Variable::Variable(TypePtr type, std::string name)
    : Global(std::move(type), std::move(name))
{
}

std::ostream &scc::ir::Variable::Print(std::ostream &stream) const
{
    return m_Type->Print(stream) << " @" << m_Name;
}

#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::Function::Function(TypePtr type, std::string name)
    : Global(std::move(type), std::move(name))
{
}

std::ostream &scc::ir::Function::Print(std::ostream &stream) const
{
    return m_Type->Print(stream) << " @" << m_Name;
}

void scc::ir::Function::Append(BlockPtr block)
{
    m_Blocks.emplace_back(std::move(block));
}

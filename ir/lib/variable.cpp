#include <scc/ir/variable.hpp>

scc::ir::Variable::Variable(Type *type, std::string name, Constant *initializer)
    : Global(type, std::move(name)),
      m_Initializer(initializer)
{
}

std::ostream &scc::ir::Variable::Print(std::ostream &stream) const
{
    return m_Initializer->PrintOperand(stream << "variable @" << m_Name << " = ");
}

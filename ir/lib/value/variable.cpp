#include <scc/ir/context.hpp>
#include <scc/ir/value.hpp>

scc::ir::Variable::Variable(TypePtr type, std::string name, ConstantPtr initializer)
    : Global(type->GetContext().GetPointerType(type), std::move(name)),
      m_Initializer(std::move(initializer))
{
}

std::ostream &scc::ir::Variable::Print(std::ostream &stream) const
{
    return m_Initializer->PrintOperand(stream << "variable @" << m_Name << " = ");
}

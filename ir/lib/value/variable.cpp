#include <scc/ir/context.hpp>
#include <scc/ir/value.hpp>

scc::ir::Variable::Variable(TypeFwd::Ptr type, std::string name, ConstantFwd::Ptr initializer)
    : Global(type->GetContext().GetPointerType(type), std::move(name)),
      m_Initializer(std::move(initializer))
{
}

std::ostream &scc::ir::Variable::Print(std::ostream &stream) const
{
    return m_Initializer->PrintOperand(stream << "variable @" << m_Name << " = ");
}

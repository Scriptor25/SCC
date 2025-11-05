#include <scc/ir/instruction.hpp>

scc::ir::ComparatorInstruction::ComparatorInstruction(
    TypePtr type,
    std::string name,
    const Comparator comparator,
    ValuePtr lhs,
    ValuePtr rhs)
    : Instruction(std::move(type), std::move(name)),
      m_Comparator(comparator),
      m_LHS(std::move(lhs)),
      m_RHS(std::move(rhs))
{
}

std::ostream &scc::ir::ComparatorInstruction::Print(std::ostream &stream) const
{
    m_Type->Print(stream << '%' << m_Name << " = ") << " cmp.";
    switch (m_Comparator)
    {
    case Comparator_LT:
        stream << "lt";
        break;
    case Comparator_GT:
        stream << "gt";
        break;
    case Comparator_LE:
        stream << "le";
        break;
    case Comparator_GE:
        stream << "ge";
        break;
    case Comparator_EQ:
        stream << "eq";
        break;
    case Comparator_NE:
        stream << "ne";
        break;
    }
    return m_RHS->Print(m_LHS->Print(stream << ' ') << ", ");
}

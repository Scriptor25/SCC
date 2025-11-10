#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::ComparatorInstruction::ComparatorInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    const Comparator comparator,
    ValueFwd::Ptr lhs,
    ValueFwd::Ptr rhs)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Comparator(comparator),
      m_LHS(std::move(lhs)),
      m_RHS(std::move(rhs))
{
    m_LHS->Use();
    m_RHS->Use();
}

scc::ir::ComparatorInstruction::~ComparatorInstruction()
{
    m_LHS->Drop();
    m_RHS->Drop();
}

std::ostream &scc::ir::ComparatorInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    stream << "cmp.";
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
    return m_RHS->PrintOperand(m_LHS->PrintOperand(stream << ' ') << ", ");
}

scc::ir::Comparator scc::ir::ComparatorInstruction::GetComparator() const
{
    return m_Comparator;
}

scc::ir::ValueFwd::Ptr scc::ir::ComparatorInstruction::GetLHS() const
{
    return m_LHS;
}

scc::ir::ValueFwd::Ptr scc::ir::ComparatorInstruction::GetRHS() const
{
    return m_RHS;
}

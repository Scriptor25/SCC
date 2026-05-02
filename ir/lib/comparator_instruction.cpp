#include <scc/ir/instruction.hpp>

#include <ostream>

scc::ir::ComparatorInstruction::ComparatorInstruction(
    Type *type,
    Block *block,
    std::string name,
    const Comparator comparator,
    Value *lhs,
    Value *rhs)
    : Instruction(type, block, std::move(name)),
      m_Comparator(comparator),
      m_LHS(lhs),
      m_RHS(rhs)
{
    m_LHS->Use(this);
    m_RHS->Use(this);
}

scc::ir::ComparatorInstruction::~ComparatorInstruction()
{
    DropAll();
}

void scc::ir::ComparatorInstruction::DropAll()
{
    if (m_LHS)
    {
        m_LHS->Drop(this);
        m_LHS = nullptr;
    }
    if (m_RHS)
    {
        m_RHS->Drop(this);
        m_RHS = nullptr;
    }
}

void scc::ir::ComparatorInstruction::Replace(Value *value, Value *with)
{
    if (m_LHS == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_LHS = with;
        return;
    }

    if (m_RHS == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_RHS = with;
        return;
    }

    Instruction::Replace(value, with);
}

std::ostream &scc::ir::ComparatorInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    stream << "cmp.";
    switch (m_Comparator)
    {
    case Comparator::SLT:
        stream << "slt";
        break;
    case Comparator::ULT:
        stream << "ult";
        break;
    case Comparator::SGT:
        stream << "sgt";
        break;
    case Comparator::UGT:
        stream << "ugt";
        break;
    case Comparator::SLE:
        stream << "sle";
        break;
    case Comparator::ULE:
        stream << "ule";
        break;
    case Comparator::SGE:
        stream << "sge";
        break;
    case Comparator::UGE:
        stream << "uge";
        break;
    case Comparator::EQ:
        stream << "eq";
        break;
    case Comparator::NE:
        stream << "ne";
        break;
    }

    return m_RHS->PrintOperand(m_LHS->PrintOperand(stream << ' ') << ", ");
}

scc::ir::Comparator scc::ir::ComparatorInstruction::GetComparator() const
{
    return m_Comparator;
}

scc::ir::Value *scc::ir::ComparatorInstruction::GetLHS() const
{
    return m_LHS;
}

scc::ir::Value *scc::ir::ComparatorInstruction::GetRHS() const
{
    return m_RHS;
}

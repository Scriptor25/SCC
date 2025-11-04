#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::Instruction::Instruction(
    TypePtr type,
    std::string name)
    : Value(std::move(type)),
      m_Name(std::move(name))
{
}

scc::ir::OperatorInstruction::OperatorInstruction(
    TypePtr type,
    std::string name,
    const Operator operator_,
    std::vector<ValuePtr> operands)
    : Instruction(std::move(type), std::move(name)),
      m_Operator(operator_),
      m_Operands(std::move(operands))
{
}

std::ostream &scc::ir::OperatorInstruction::Print(std::ostream &stream) const
{
    m_Type->Print(stream << '%' << m_Name << " = ") << " op.";
    switch (m_Operator)
    {
    case Operator_Add:
        stream << "add";
        break;
    case Operator_Sub:
        stream << "sub";
        break;
    case Operator_Mul:
        stream << "mul";
        break;
    case Operator_Div:
        stream << "div";
        break;
    case Operator_Rem:
        stream << "rem";
        break;
    case Operator_And:
        stream << "and";
        break;
    case Operator_Or:
        stream << "or";
        break;
    case Operator_Xor:
        stream << "xor";
        break;
    }
    stream << ' ';
    for (auto i = m_Operands.begin(); i != m_Operands.end(); ++i)
    {
        if (i != m_Operands.begin())
        {
            stream << ", ";
        }
        (*i)->Print(stream);
    }
    return stream;
}

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

scc::ir::BranchInstruction::BranchInstruction(BlockPtr destination)
    : Instruction(nullptr, ""),
      m_Condition(nullptr),
      m_Then(std::move(destination)),
      m_Else(nullptr)
{
}

scc::ir::BranchInstruction::BranchInstruction(ValuePtr condition, BlockPtr then, BlockPtr else_)
    : Instruction(nullptr, ""),
      m_Condition(std::move(condition)),
      m_Then(std::move(then)),
      m_Else(std::move(else_))
{
}

std::ostream &scc::ir::BranchInstruction::Print(std::ostream &stream) const
{
    if (m_Condition)
    {
        return m_Else->Print(m_Then->Print(m_Condition->Print(stream << "br ") << ", ") << ", ");
    }
    return m_Then->Print(stream << "br ");
}

#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::OperatorInstruction::OperatorInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    const Operator operator_,
    std::vector<ValueFwd::Ptr> operands)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Operator(operator_),
      m_Operands(std::move(operands))
{
    for (const auto &operand : m_Operands)
        operand->Use();
}

scc::ir::OperatorInstruction::~OperatorInstruction()
{
    for (const auto &operand : m_Operands)
        operand->Drop();
}

std::ostream &scc::ir::OperatorInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        m_Register->Print(stream) << " = ";
    switch (m_Operator)
    {
    case Operator::Add:
        stream << "add";
        break;
    case Operator::Sub:
        stream << "sub";
        break;
    case Operator::Mul:
        stream << "mul";
        break;
    case Operator::SDiv:
        stream << "sdiv";
        break;
    case Operator::UDiv:
        stream << "udiv";
        break;
    case Operator::SRem:
        stream << "srem";
        break;
    case Operator::URem:
        stream << "urem";
        break;
    case Operator::And:
        stream << "and";
        break;
    case Operator::Or:
        stream << "or";
        break;
    case Operator::Xor:
        stream << "xor";
        break;
    }
    stream << ' ';
    for (auto i = m_Operands.begin(); i != m_Operands.end(); ++i)
    {
        if (i != m_Operands.begin())
            stream << ", ";
        (*i)->PrintOperand(stream);
    }
    return stream;
}

scc::ir::Operator scc::ir::OperatorInstruction::GetOperator() const
{
    return m_Operator;
}

unsigned scc::ir::OperatorInstruction::GetOperandCount() const
{
    return m_Operands.size();
}

scc::ir::ValueFwd::Ptr scc::ir::OperatorInstruction::GetOperand(const unsigned index) const
{
    return m_Operands[index];
}

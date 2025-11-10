#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::OperatorInstruction::OperatorInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    const Operator operator_,
    std::vector<ValueFwd::Ptr> operands)
    : IdentifiedInstruction(std::move(type), std::move(register_), std::move(block)),
      m_Operator(operator_),
      m_Operands(std::move(operands))
{
}

std::ostream &scc::ir::OperatorInstruction::Print(std::ostream &stream) const
{
    m_Register->Print(stream) << " = ";
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
    return m_Operands.at(index);
}

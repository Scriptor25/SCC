#include <scc/ir/instruction.hpp>

scc::ir::OperatorInstruction::OperatorInstruction(
    TypePtr type,
    std::string name,
    Block::WeakPtr block,
    const Operator operator_,
    std::vector<ValuePtr> operands)
    : Instruction(std::move(type), std::move(name), std::move(block)),
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

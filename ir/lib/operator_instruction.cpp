#include <scc/ir/instruction.hpp>

#include <ostream>

scc::ir::OperatorInstruction::OperatorInstruction(
    Type *type,
    Block *block,
    std::string name,
    const Operator operator_,
    std::vector<Value *> operands)
    : Instruction(type, block, std::move(name)),
      m_Operator(operator_),
      m_Operands(std::move(operands))
{
    for (auto *operand : m_Operands)
        operand->Use(this);
}

scc::ir::OperatorInstruction::~OperatorInstruction()
{
    DropAll();
}

void scc::ir::OperatorInstruction::DropAll()
{
    for (auto &operand : m_Operands)
        if (operand)
        {
            operand->Drop(this);
            operand = nullptr;
        }

    m_Operands.clear();
}

void scc::ir::OperatorInstruction::Replace(Value *value, Value *with)
{
    for (auto &operand : m_Operands)
        if (operand == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            operand = with;
        }
}

std::ostream &scc::ir::OperatorInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

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
        const auto first = i == m_Operands.begin();

        if (!first)
            stream << ", ";

        (*i)->PrintOperand(stream, first);
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

scc::ir::Value *scc::ir::OperatorInstruction::GetOperand(const unsigned index) const
{
    return m_Operands[index];
}

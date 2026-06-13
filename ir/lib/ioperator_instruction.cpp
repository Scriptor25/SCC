#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>

#include <ostream>

const std::unordered_map<scc::ir::IOperator, std::string_view> scc::ir::IOperatorToString
{
    { IOperator::ADD, "add" },
    { IOperator::SUB, "sub" },
    { IOperator::MUL, "mul" },
    { IOperator::SDIV, "sdiv" },
    { IOperator::UDIV, "udiv" },
    { IOperator::SREM, "srem" },
    { IOperator::UREM, "urem" },
    { IOperator::AND, "and" },
    { IOperator::OR, "or" },
    { IOperator::XOR, "xor" },
};

const std::unordered_map<std::string_view, scc::ir::IOperator> scc::ir::StringToIOperator
{
    { "add", IOperator::ADD },
    { "sub", IOperator::SUB },
    { "mul", IOperator::MUL },
    { "sdiv", IOperator::SDIV },
    { "udiv", IOperator::UDIV },
    { "srem", IOperator::SREM },
    { "urem", IOperator::UREM },
    { "and", IOperator::AND },
    { "or", IOperator::OR },
    { "xor", IOperator::XOR },
};

scc::ir::IOperatorInstruction::IOperatorInstruction(
    Type *type,
    Block *block,
    std::string name,
    const IOperator operator_,
    std::vector<Value *> operands)
    : Instruction(type, block, std::move(name)),
      m_Operator(operator_),
      m_Operands(std::move(operands))
{
    for (auto *operand : m_Operands)
        operand->Use(this);
}

scc::ir::IOperatorInstruction::~IOperatorInstruction()
{
    DropAll();
}

void scc::ir::IOperatorInstruction::DropAll()
{
    for (auto &operand : m_Operands)
        if (operand)
        {
            operand->Drop(this);
            operand = {};
        }

    m_Operands.clear();
}

void scc::ir::IOperatorInstruction::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::IOperatorInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    stream << IOperatorToString.at(m_Operator) << ' ';

    for (auto i = m_Operands.begin(); i != m_Operands.end(); ++i)
    {
        const auto first = i == m_Operands.begin();

        if (!first)
            stream << ", ";

        (*i)->PrintOperand(stream, first);
    }

    return stream;
}

std::ostream &scc::ir::IOperatorInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
}

scc::ir::IOperator scc::ir::IOperatorInstruction::GetOperator() const
{
    return m_Operator;
}

unsigned scc::ir::IOperatorInstruction::GetOperandCount() const
{
    return m_Operands.size();
}

scc::ir::Value *scc::ir::IOperatorInstruction::GetOperand(const unsigned index) const
{
    return m_Operands[index];
}

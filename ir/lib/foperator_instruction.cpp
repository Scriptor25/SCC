#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>

#include <ostream>

const std::unordered_map<scc::ir::FOperator, std::string_view> scc::ir::FOperatorToString
{
    { FOperator::ADD, "fadd" },
    { FOperator::SUB, "fsub" },
    { FOperator::MUL, "fmul" },
    { FOperator::DIV, "fdiv" },
    { FOperator::REM, "frem" },
};

const std::unordered_map<std::string_view, scc::ir::FOperator> scc::ir::StringToFOperator
{
    { "fadd", FOperator::ADD },
    { "fsub", FOperator::SUB },
    { "fmul", FOperator::MUL },
    { "fdiv", FOperator::DIV },
    { "frem", FOperator::REM },
};

scc::ir::FOperatorInstruction::FOperatorInstruction(
    Type *type,
    Block *block,
    std::string name,
    const FOperator operator_,
    std::vector<Value *> operands)
    : Instruction(type, block, std::move(name)),
      m_Operator(operator_),
      m_Operands(std::move(operands))
{
    for (auto *operand : m_Operands)
        operand->Use(this);
}

scc::ir::FOperatorInstruction::~FOperatorInstruction()
{
    DropAll();
}

void scc::ir::FOperatorInstruction::DropAll()
{
    for (auto &operand : m_Operands)
        if (operand)
        {
            operand->Drop(this);
            operand = {};
        }

    m_Operands.clear();
}

void scc::ir::FOperatorInstruction::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::FOperatorInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    stream << FOperatorToString.at(m_Operator) << ' ';

    for (auto i = m_Operands.begin(); i != m_Operands.end(); ++i)
    {
        const auto first = i == m_Operands.begin();

        if (!first)
            stream << ", ";

        (*i)->PrintOperand(stream, first);
    }

    return stream;
}

std::ostream &scc::ir::FOperatorInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
}

scc::ir::FOperator scc::ir::FOperatorInstruction::GetOperator() const
{
    return m_Operator;
}

unsigned scc::ir::FOperatorInstruction::GetOperandCount() const
{
    return m_Operands.size();
}

scc::ir::Value *scc::ir::FOperatorInstruction::GetOperand(const unsigned index) const
{
    return m_Operands[index];
}

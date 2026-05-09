#include <scc/as/instruction.hpp>

scc::as::Instruction::Instruction(const InstructionCode code, std::vector<Operand> operands)
    : m_Code(code),
      m_Operands(std::move(operands))
{
}

void scc::as::Instruction::SetCode(const InstructionCode code)
{
    m_Code = code;
}

scc::as::InstructionCode scc::as::Instruction::GetCode() const
{
    return m_Code;
}

size_t scc::as::Instruction::GetOperandCount() const
{
    return m_Operands.size();
}

scc::as::Operand &scc::as::Instruction::operator[](const size_t index)
{
    return m_Operands[index];
}

const scc::as::Operand &scc::as::Instruction::operator[](const size_t index) const
{
    return m_Operands[index];
}

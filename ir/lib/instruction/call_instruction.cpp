#include <scc/ir/instruction.hpp>

scc::ir::CallInstruction::CallInstruction(
    TypePtr type,
    std::string name,
    Block::WeakPtr block,
    ValuePtr callee,
    std::vector<ValuePtr> arguments)
    : NamedInstruction(std::move(type), std::move(name), std::move(block)),
      m_Callee(std::move(callee)),
      m_Arguments(std::move(arguments))
{
}

std::ostream &scc::ir::CallInstruction::Print(std::ostream &stream) const
{
    m_Callee->PrintOperand(stream << '%' << m_Name << " = call ");
    for (auto &argument : m_Arguments)
    {
        argument->PrintOperand(stream << ", ");
    }
    return stream;
}

scc::ir::CallVoidInstruction::CallVoidInstruction(
    Block::WeakPtr block,
    ValuePtr callee,
    std::vector<ValuePtr> arguments)
    : Instruction(std::move(block)),
      m_Callee(std::move(callee)),
      m_Arguments(std::move(arguments))
{
}

std::ostream &scc::ir::CallVoidInstruction::Print(std::ostream &stream) const
{
    m_Callee->PrintOperand(stream << "call ");
    for (auto &argument : m_Arguments)
    {
        argument->PrintOperand(stream << ", ");
    }
    return stream;
}

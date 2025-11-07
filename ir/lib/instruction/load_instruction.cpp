#include <scc/ir/instruction.hpp>

scc::ir::LoadInstruction::LoadInstruction(TypePtr type, std::string name, Block::WeakPtr block, ValuePtr pointer)
    : NamedInstruction(std::move(type), std::move(name), std::move(block)),
      m_Pointer(std::move(pointer))
{
}

std::ostream &scc::ir::LoadInstruction::Print(std::ostream &stream) const
{
    return m_Pointer->PrintOperand(stream << '%' << m_Name << " = load ");
}

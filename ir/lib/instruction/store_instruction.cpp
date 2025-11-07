#include <scc/ir/instruction.hpp>

scc::ir::StoreInstruction::StoreInstruction(Block::WeakPtr block, ValuePtr pointer, ValuePtr value)
    : Instruction(std::move(block)),
      m_Pointer(std::move(pointer)),
      m_Value(std::move(value))
{
}

std::ostream &scc::ir::StoreInstruction::Print(std::ostream &stream) const
{
    return m_Value->PrintOperand(m_Pointer->PrintOperand(stream << "store ") << ", ");
}

#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::Instruction::Instruction(Block::WeakPtr block)
    : m_Block(std::move(block))
{
}

scc::ir::NamedInstruction::NamedInstruction(TypePtr type, std::string name, Block::WeakPtr block)
    : Instruction(std::move(block)),
      NamedValue(std::move(type), std::move(name))
{
}

std::ostream &scc::ir::NamedInstruction::PrintOperand(std::ostream &stream) const
{
    return stream << '%' << m_Name;
}

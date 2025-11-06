#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::Instruction::Instruction(
    TypePtr type,
    std::string name,
    Block::WeakPtr block)
    : Value(std::move(type)),
      m_Name(std::move(name)),
      m_Block(std::move(block))
{
}

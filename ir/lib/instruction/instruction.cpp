#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::Instruction::Instruction(BlockFwd::WeakPtr block)
    : m_Block(std::move(block))
{
}

scc::ir::BlockFwd::Ptr scc::ir::Instruction::GetBlock() const
{
    return m_Block.lock();
}

scc::ir::IdentifiedInstruction::IdentifiedInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block)
    : Instruction(std::move(block)),
      IdentifiedValue(std::move(type), std::move(register_))
{
}

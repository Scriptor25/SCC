#include <scc/ir/instruction.hpp>

scc::ir::BlockFwd::Ptr scc::ir::Instruction::GetBlock() const
{
    return m_Block.lock();
}

scc::ir::Instruction::Instruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block)
    : IdentifiedValue(std::move(type), std::move(register_)),
      m_Block(std::move(block))
{
}

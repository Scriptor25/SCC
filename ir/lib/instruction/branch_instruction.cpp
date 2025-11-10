#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::BranchInstruction::BranchInstruction(BlockFwd::WeakPtr block, Block::Ptr destination)
    : Instruction(std::move(block)),
      m_Then(std::move(destination))
{
}

scc::ir::BranchInstruction::BranchInstruction(
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr condition,
    Block::Ptr then,
    Block::Ptr else_)
    : Instruction(std::move(block)),
      m_Condition(std::move(condition)),
      m_Then(std::move(then)),
      m_Else(std::move(else_))
{
}

std::ostream &scc::ir::BranchInstruction::Print(std::ostream &stream) const
{
    if (m_Condition)
    {
        return m_Else->PrintOperand(m_Then->PrintOperand(m_Condition->PrintOperand(stream << "br ") << ", ") << ", ");
    }
    return m_Then->PrintOperand(stream << "br ");
}

scc::ir::ValueFwd::Ptr scc::ir::BranchInstruction::GetCondition() const
{
    return m_Condition;
}

scc::ir::BlockFwd::Ptr scc::ir::BranchInstruction::GetThen() const
{
    return m_Then;
}

scc::ir::BlockFwd::Ptr scc::ir::BranchInstruction::GetElse() const
{
    return m_Else;
}

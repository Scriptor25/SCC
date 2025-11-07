#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::BranchInstruction::BranchInstruction(Block::WeakPtr block, BlockPtr destination)
    : Instruction(std::move(block)),
      m_Condition(nullptr),
      m_Then(std::move(destination)),
      m_Else(nullptr)
{
}

scc::ir::BranchInstruction::BranchInstruction(Block::WeakPtr block, ValuePtr condition, BlockPtr then, BlockPtr else_)
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

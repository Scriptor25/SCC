#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::BranchInstruction::BranchInstruction(BlockPtr destination)
    : Instruction(nullptr, ""),
      m_Condition(nullptr),
      m_Then(std::move(destination)),
      m_Else(nullptr)
{
}

scc::ir::BranchInstruction::BranchInstruction(ValuePtr condition, BlockPtr then, BlockPtr else_)
    : Instruction(nullptr, ""),
      m_Condition(std::move(condition)),
      m_Then(std::move(then)),
      m_Else(std::move(else_))
{
}

std::ostream &scc::ir::BranchInstruction::Print(std::ostream &stream) const
{
    if (m_Condition)
    {
        return m_Else->Print(m_Then->Print(m_Condition->Print(stream << "br ") << ", ") << ", ");
    }
    return m_Then->Print(stream << "br ");
}

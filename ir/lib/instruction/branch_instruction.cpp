#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::BranchInstruction::BranchInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    Block::Ptr destination)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Then(std::move(destination))
{
    m_Then->Use();
}

scc::ir::BranchInstruction::BranchInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr condition,
    Block::Ptr then,
    Block::Ptr else_)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Condition(std::move(condition)),
      m_Then(std::move(then)),
      m_Else(std::move(else_))
{
    m_Condition->Use();
    m_Then->Use();
    m_Else->Use();
}

scc::ir::BranchInstruction::~BranchInstruction()
{
    if (m_Condition)
    {
        m_Condition->Drop();
        m_Then->Drop();
        m_Else->Drop();
    }
    else
    {
        m_Then->Drop();
    }
}

std::ostream &scc::ir::BranchInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    stream << "br ";
    if (m_Condition)
    {
        return m_Else->PrintOperand(m_Then->PrintOperand(m_Condition->PrintOperand(stream) << ", ") << ", ");
    }
    return m_Then->PrintOperand(stream);
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

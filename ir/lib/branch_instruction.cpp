#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>

scc::ir::BranchInstruction::BranchInstruction(
    Type *type,
    Block *block,
    Value *condition,
    Block *then,
    Block *else_)
    : Instruction(type, block),
      m_Condition(condition),
      m_Then(then),
      m_Else(else_)
{
    m_Condition->Use(this);
    m_Then->Use(this);
    m_Else->Use(this);
}

scc::ir::BranchInstruction::~BranchInstruction()
{
    DropAll();
}

void scc::ir::BranchInstruction::DropAll()
{
    if (m_Condition)
    {
        m_Condition->Drop(this);
        m_Condition = nullptr;
    }
    if (m_Then)
    {
        m_Then->Drop(this);
        m_Then = nullptr;
    }
    if (m_Else)
    {
        m_Else->Drop(this);
        m_Else = nullptr;
    }
}

void scc::ir::BranchInstruction::Replace(Value *value, Value *with)
{
    if (m_Condition == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Condition = with;
        return;
    }

    if (m_Then == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Then = dynamic_cast<Block *>(with);
        return;
    }

    if (m_Else == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Else = dynamic_cast<Block *>(with);
        return;
    }

    Instruction::Replace(value, with);
}

std::ostream &scc::ir::BranchInstruction::Print(std::ostream &stream) const
{
    return m_Else->PrintOperand(m_Then->PrintOperand(m_Condition->PrintOperand(stream << "br ") << ", ") << ", ");
}

bool scc::ir::BranchInstruction::IsTerminator() const
{
    return true;
}

size_t scc::ir::BranchInstruction::GetSuccessorCount() const
{
    return 2ull;
}

scc::ir::Block *scc::ir::BranchInstruction::GetSuccessor(size_t index) const
{
    AssertIndexInBounds(index, 2ull);

    return index == 0ull ? m_Then : m_Else;
}

scc::ir::Value *scc::ir::BranchInstruction::GetCondition() const
{
    return m_Condition;
}

scc::ir::Block *scc::ir::BranchInstruction::GetThen() const
{
    return m_Then;
}

scc::ir::Block *scc::ir::BranchInstruction::GetElse() const
{
    return m_Else;
}

#include <scc/assert.hpp>
#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::DirectBranchInstruction::DirectBranchInstruction(
    Type *type,
    Block *block,
    Block *destination)
    : Instruction(type, block),
      m_Destination(destination)
{
    m_Destination->Use(this);
}

scc::ir::DirectBranchInstruction::~DirectBranchInstruction()
{
    DropAll();
}

void scc::ir::DirectBranchInstruction::DropAll()
{
    if (m_Destination)
    {
        m_Destination->Drop(this);
        m_Destination = nullptr;
    }
}

void scc::ir::DirectBranchInstruction::Replace(Value *value, Value *with)
{
    if (m_Destination == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Destination = dynamic_cast<Block *>(with);
    }
}

std::ostream &scc::ir::DirectBranchInstruction::Print(std::ostream &stream) const
{
    return m_Destination->PrintOperand(stream << "br ", false);
}

bool scc::ir::DirectBranchInstruction::IsTerminator() const
{
    return true;
}

size_t scc::ir::DirectBranchInstruction::GetSuccessorCount() const
{
    return 1;
}

scc::ir::Block *scc::ir::DirectBranchInstruction::GetSuccessor(const size_t index) const
{
    AssertIndexInBounds(index, 1);

    return m_Destination;
}

scc::ir::Block *scc::ir::DirectBranchInstruction::GetDestination() const
{
    return m_Destination;
}

#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>

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
        m_Destination = {};
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

std::ostream &scc::ir::DirectBranchInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
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
    AssertInBounds(index, 1);

    return m_Destination;
}

scc::ir::Block *scc::ir::DirectBranchInstruction::GetDestination() const
{
    return m_Destination;
}

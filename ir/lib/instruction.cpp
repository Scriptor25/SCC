#include <scc/assert.hpp>
#include <scc/error.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

scc::ir::Instruction::Instruction(Type *type, Block *block)
    : User(type),
      m_Block(block)
{
}

scc::ir::Instruction::Instruction(Type *type, Block *block, std::string name)
    : User(type),
      m_Block(block),
      m_Name(std::move(name))
{
}

std::ostream &scc::ir::Instruction::PrintOperand(std::ostream &stream) const
{
    Assert(!m_Name.empty(), "cannot use unnamed instruction value");

    return stream << '%' << m_Name;
}

scc::ir::Block *scc::ir::Instruction::GetBlock() const
{
    return m_Block;
}

void scc::ir::Instruction::SetName(std::string name)
{
    m_Name = std::move(name);
}

const std::string &scc::ir::Instruction::GetName() const
{
    return m_Name;
}

bool scc::ir::Instruction::IsTerminator() const
{
    return false;
}

size_t scc::ir::Instruction::GetSuccessorCount() const
{
    return 0;
}

scc::ir::Block *scc::ir::Instruction::GetSuccessor(const size_t index) const
{
    ErrorIndexOutOfBounds(index, 0);
}

#include <scc/ir/block.hpp>

scc::ir::Block::Block(std::string name, Function::WeakPtr function)
    : m_Name(std::move(name)),
      m_Function(std::move(function))
{
}

std::ostream &scc::ir::Block::Print(std::ostream &stream) const
{
    return stream << '$' << m_Name;
}

scc::ir::Shared<scc::ir::Function>::Ptr scc::ir::Block::GetFunction() const
{
    return m_Function.lock();
}

std::string scc::ir::Block::GetName() const
{
    return m_Name;
}

unsigned scc::ir::Block::GetInstructionCount() const
{
    return m_Instructions.size();
}

scc::ir::InstructionPtr scc::ir::Block::GetInstruction(const unsigned index) const
{
    return m_Instructions.at(index);
}

std::vector<scc::ir::InstructionPtr>::const_iterator scc::ir::Block::begin() const
{
    return m_Instructions.begin();
}

std::vector<scc::ir::InstructionPtr>::const_iterator scc::ir::Block::end() const
{
    return m_Instructions.end();
}

void scc::ir::Block::Insert(InstructionPtr instruction)
{
    m_Instructions.emplace_back(std::move(instruction));
}

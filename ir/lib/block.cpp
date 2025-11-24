#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::Block::Block(std::string name, Function::WeakPtr function)
    : m_Name(std::move(name)),
      m_Function(std::move(function))
{
}

std::string scc::ir::Block::GetName() const
{
    return m_Name;
}

void scc::ir::Block::SetName(std::string name)
{
    m_Name = std::move(name);
}

std::ostream &scc::ir::Block::Print(std::ostream &stream) const
{
    stream << m_Name << ':';
    if (!m_Predecessors.empty())
    {
        stream << " ; ";
        for (auto i = m_Predecessors.begin(); i != m_Predecessors.end(); ++i)
        {
            if (i != m_Predecessors.begin())
            {
                stream << ", ";
            }
            stream << (*i)->GetName();
        }
    }
    for (auto &instruction : m_Instructions)
    {
        instruction->Print(stream << std::endl << "    ");
    }
    return stream;
}

std::ostream &scc::ir::Block::PrintOperand(std::ostream &stream) const
{
    return stream << '%' << m_Name;
}

scc::ir::Function::Ptr scc::ir::Block::GetFunction() const
{
    return m_Function.lock();
}

unsigned scc::ir::Block::GetInstructionCount() const
{
    return m_Instructions.size();
}

scc::ir::InstructionFwd::Ptr scc::ir::Block::GetInstruction(const unsigned index) const
{
    return m_Instructions.at(index);
}

std::vector<scc::ir::InstructionFwd::Ptr>::const_iterator scc::ir::Block::begin() const
{
    return m_Instructions.begin();
}

std::vector<scc::ir::InstructionFwd::Ptr>::const_iterator scc::ir::Block::end() const
{
    return m_Instructions.end();
}

void scc::ir::Block::Insert(InstructionFwd::Ptr instruction)
{
    m_Instructions.emplace_back(std::move(instruction));
}

void scc::ir::Block::UsePred(Ptr block)
{
    m_Predecessors.emplace(std::move(block));
}

void scc::ir::Block::DropPred(Ptr block)
{
    m_Predecessors.erase(std::move(block));
}

scc::ir::RegisterFwd::Ptr scc::ir::Block::CreateRegister(std::string name) const
{
    return m_Function.lock()->CreateRegister(std::move(name));
}

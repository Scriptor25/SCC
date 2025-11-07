#include <scc/ir/instruction.hpp>

scc::ir::SelectInstruction::SelectInstruction(
    TypePtr type,
    std::string name,
    Block::WeakPtr block,
    std::vector<std::pair<BlockPtr, ValuePtr>> options)
    : NamedInstruction(std::move(type), std::move(name), std::move(block)),
      m_Options(std::move(options))
{
}

std::ostream &scc::ir::SelectInstruction::Print(std::ostream &stream) const
{
    stream << '%' << m_Name << " = select ";
    for (auto i = m_Options.begin(); i != m_Options.end(); ++i)
    {
        if (i != m_Options.begin())
        {
            stream << ", ";
        }
        i->second->PrintOperand(i->first->PrintOperand(stream << "[ ") << ", ") << " ]";
    }
    return stream;
}

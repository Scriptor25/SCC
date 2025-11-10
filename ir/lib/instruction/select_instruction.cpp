#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::SelectInstruction::SelectInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> options)
    : IdentifiedInstruction(std::move(type), std::move(register_), std::move(block)),
      m_Options(std::move(options))
{
}

std::ostream &scc::ir::SelectInstruction::Print(std::ostream &stream) const
{
    m_Register->Print(stream) << " = select ";
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

unsigned scc::ir::SelectInstruction::GetOptionCount() const
{
    return m_Options.size();
}

std::pair<scc::ir::BlockFwd::Ptr, scc::ir::ValueFwd::Ptr> scc::ir::SelectInstruction::GetOption(
    const unsigned index) const
{
    return m_Options.at(index);
}

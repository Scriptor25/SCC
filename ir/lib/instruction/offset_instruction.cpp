#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::OffsetInstruction::OffsetInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr base,
    std::vector<ValueFwd::Ptr> offsets)
    : IdentifiedInstruction(std::move(type), std::move(register_), std::move(block)),
      m_Base(std::move(base)),
      m_Offsets(std::move(offsets))
{
}

std::ostream &scc::ir::OffsetInstruction::Print(std::ostream &stream) const
{
    m_Base->PrintOperand(m_Type->Print(m_Register->Print(stream) << " = ") << " offset ");
    for (auto &offset : m_Offsets)
    {
        offset->PrintOperand(stream << ", ");
    }
    return stream;
}

scc::ir::ValueFwd::Ptr scc::ir::OffsetInstruction::GetBase() const
{
    return m_Base;
}

unsigned scc::ir::OffsetInstruction::GetOffsetCount() const
{
    return m_Offsets.size();
}

scc::ir::ValueFwd::Ptr scc::ir::OffsetInstruction::GetOffset(const unsigned index) const
{
    return m_Offsets.at(index);
}

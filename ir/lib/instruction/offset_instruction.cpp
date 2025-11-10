#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::OffsetInstruction::OffsetInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr base,
    std::vector<ValueFwd::Ptr> offsets)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Base(std::move(base)),
      m_Offsets(std::move(offsets))
{
    m_Base->Use();
    for (const auto &offset : m_Offsets)
    {
        offset->Use();
    }
}

scc::ir::OffsetInstruction::~OffsetInstruction()
{
    m_Base->Drop();
    for (const auto &offset : m_Offsets)
    {
        offset->Drop();
    }
}

std::ostream &scc::ir::OffsetInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    m_Base->PrintOperand(m_Type->Print(stream) << " offset ");
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

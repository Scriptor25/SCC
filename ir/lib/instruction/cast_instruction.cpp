#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::CastInstruction::CastInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr value)
    : IdentifiedInstruction(std::move(type), std::move(register_), std::move(block)),
      m_Value(std::move(value))
{
}

std::ostream &scc::ir::CastInstruction::Print(std::ostream &stream) const
{
    return m_Value->PrintOperand(m_Type->Print(m_Register->Print(stream) << " = ") << " cast ");
}

scc::ir::ValueFwd::Ptr scc::ir::CastInstruction::GetValue() const
{
    return m_Value;
}

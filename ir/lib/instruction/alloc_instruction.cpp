#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::AllocInstruction::AllocInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    const unsigned count)
    : IdentifiedInstruction(std::move(type), std::move(register_), std::move(block)),
      m_Count(count)
{
}

std::ostream &scc::ir::AllocInstruction::Print(std::ostream &stream) const
{
    const auto type = std::dynamic_pointer_cast<PointerType>(m_Type)->GetBase();
    return type->Print(m_Register->Print(stream) << " = ") << " alloc " << m_Count;
}

unsigned scc::ir::AllocInstruction::GetCount() const
{
    return m_Count;
}

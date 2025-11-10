#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::LoadInstruction::LoadInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr pointer)
    : IdentifiedInstruction(std::move(type), std::move(register_), std::move(block)),
      m_Pointer(std::move(pointer))
{
}

std::ostream &scc::ir::LoadInstruction::Print(std::ostream &stream) const
{
    return m_Pointer->PrintOperand(m_Register->Print(stream) << " = load ");
}

scc::ir::ValueFwd::Ptr scc::ir::LoadInstruction::GetPointer() const
{
    return m_Pointer;
}

#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::LoadInstruction::LoadInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr pointer)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Pointer(std::move(pointer))
{
    m_Pointer->Use();
}

scc::ir::LoadInstruction::~LoadInstruction()
{
    m_Pointer->Drop();
}

std::ostream &scc::ir::LoadInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    return m_Pointer->PrintOperand(stream << "load ");
}

scc::ir::ValueFwd::Ptr scc::ir::LoadInstruction::GetPointer() const
{
    return m_Pointer;
}

#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::StoreInstruction::StoreInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr pointer,
    ValueFwd::Ptr value)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Pointer(std::move(pointer)),
      m_Value(std::move(value))
{
    m_Pointer->Use();
    m_Value->Use();
}

scc::ir::StoreInstruction::~StoreInstruction()
{
    m_Pointer->Drop();
    m_Value->Drop();
}

std::ostream &scc::ir::StoreInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    return m_Value->PrintOperand(m_Pointer->PrintOperand(stream << "store ") << ", ");
}

scc::ir::ValueFwd::Ptr scc::ir::StoreInstruction::GetPointer() const
{
    return m_Pointer;
}

scc::ir::ValueFwd::Ptr scc::ir::StoreInstruction::GetValue() const
{
    return m_Value;
}

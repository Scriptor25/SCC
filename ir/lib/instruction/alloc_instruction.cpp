#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::AllocInstruction::AllocInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    const unsigned count)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Count(count)
{
}

scc::ir::AllocInstruction::~AllocInstruction()
{
}

std::ostream &scc::ir::AllocInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    const auto type = std::dynamic_pointer_cast<PointerType>(m_Type)->GetBase();
    return type->Print(stream) << " alloc " << m_Count;
}

unsigned scc::ir::AllocInstruction::GetCount() const
{
    return m_Count;
}

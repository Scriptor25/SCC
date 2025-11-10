#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::ReturnInstruction::ReturnInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr value)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Value(std::move(value))
{
    if (m_Value)
    {
        m_Value->Use();
    }
}

scc::ir::ReturnInstruction::~ReturnInstruction()
{
    if (m_Value)
    {
        m_Value->Drop();
    }
}

std::ostream &scc::ir::ReturnInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    stream << "ret ";
    if (m_Value)
    {
        return m_Value->PrintOperand(stream);
    }
    return stream << "void";
}

scc::ir::ValueFwd::Ptr scc::ir::ReturnInstruction::GetValue() const
{
    return m_Value;
}

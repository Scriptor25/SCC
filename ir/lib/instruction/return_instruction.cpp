#include <scc/ir/instruction.hpp>

scc::ir::RetInstruction::RetInstruction(BlockFwd::WeakPtr block)
    : Instruction(std::move(block))
{
}

scc::ir::RetInstruction::RetInstruction(BlockFwd::WeakPtr block, ValueFwd::Ptr value)
    : Instruction(std::move(block)),
      m_Value(std::move(value))
{
}

std::ostream &scc::ir::RetInstruction::Print(std::ostream &stream) const
{
    if (m_Value)
    {
        return m_Value->PrintOperand(stream << "ret ");
    }
    return stream << "ret void";
}

scc::ir::ValueFwd::Ptr scc::ir::RetInstruction::GetValue() const
{
    return m_Value;
}

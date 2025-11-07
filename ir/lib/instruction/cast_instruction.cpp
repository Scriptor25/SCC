#include <scc/ir/instruction.hpp>

scc::ir::CastInstruction::CastInstruction(TypePtr type, std::string name, Block::WeakPtr block, ValuePtr value)
    : NamedInstruction(std::move(type), std::move(name), std::move(block)),
      m_Value(std::move(value))
{
}

std::ostream &scc::ir::CastInstruction::Print(std::ostream &stream) const
{
    return m_Value->PrintOperand(m_Type->Print(stream << '%' << m_Name << " = ") << " cast ");
}

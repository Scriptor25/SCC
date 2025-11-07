#include <scc/ir/instruction.hpp>

scc::ir::AllocInstruction::AllocInstruction(TypePtr type, std::string name, Block::WeakPtr block)
    : NamedInstruction(std::move(type), std::move(name), std::move(block))
{
}

std::ostream &scc::ir::AllocInstruction::Print(std::ostream &stream) const
{
    const auto type = std::dynamic_pointer_cast<PointerType>(m_Type)->GetBase();
    return type->Print(stream << '%' << m_Name << " = ") << " alloc";
}

#include <scc/ir/instruction.hpp>

scc::ir::OffsetInstruction::OffsetInstruction(
    TypePtr type,
    std::string name,
    Block::WeakPtr block,
    ValuePtr base,
    std::vector<ValuePtr> offsets)
    : NamedInstruction(std::move(type), std::move(name), std::move(block)),
      m_Base(std::move(base)),
      m_Offsets(std::move(offsets))
{
}

std::ostream &scc::ir::OffsetInstruction::Print(std::ostream &stream) const
{
    m_Base->PrintOperand(stream << '%' << m_Name << " = offset ");
    for (auto &offset : m_Offsets)
    {
        offset->PrintOperand(stream << ", ");
    }
    return stream;
}

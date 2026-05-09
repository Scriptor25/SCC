#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

#include <ostream>

scc::ir::AllocInstruction::AllocInstruction(
    PointerType *type,
    Block *block,
    std::string name,
    const unsigned count)
    : Instruction(type, block, std::move(name)),
      m_Count(count)
{
}

std::ostream &scc::ir::AllocInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    const auto *element_type = dynamic_cast<PointerType *>(m_Type)->GetElement();

    return element_type->Print(stream) << " alloc " << m_Count;
}

unsigned scc::ir::AllocInstruction::GetCount() const
{
    return m_Count;
}

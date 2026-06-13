#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

#include <ostream>

scc::ir::AllocInstruction::AllocInstruction(
    PointerType *type,
    Block *block,
    std::string name,
    const uint64_t count)
    : Instruction(type, block, std::move(name)),
      m_Count(count)
{
}

std::ostream &scc::ir::AllocInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    const auto *element_type = m_Type->GetElement();
    element_type->Print(stream << "alloc ");

    if (m_Count == 1)
        return stream;

    return stream << " 0x" << std::hex << m_Count;
}

std::ostream &scc::ir::AllocInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    // context -> allocate n x size of type on stack
    // request result register
    // write offset relative to frame base to result register
    Error("TODO");
}

uint64_t scc::ir::AllocInstruction::GetCount() const
{
    return m_Count;
}

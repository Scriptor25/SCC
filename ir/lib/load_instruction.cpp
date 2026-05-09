#include <scc/ir/instruction.hpp>

#include <ostream>

scc::ir::LoadInstruction::LoadInstruction(
    Type *type,
    Block *block,
    std::string name,
    Value *pointer)
    : Instruction(type, block, std::move(name)),
      m_Pointer(pointer)
{
    m_Pointer->Use(this);
}

scc::ir::LoadInstruction::~LoadInstruction()
{
    DropAll();
}

void scc::ir::LoadInstruction::DropAll()
{
    if (m_Pointer)
    {
        m_Pointer->Drop(this);
        m_Pointer = nullptr;
    }
}

void scc::ir::LoadInstruction::Replace(Value *value, Value *with)
{
    if (m_Pointer == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Pointer = with;
    }
}

std::ostream &scc::ir::LoadInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    return m_Pointer->PrintOperand(stream << "load ", true);
}

scc::ir::Value *scc::ir::LoadInstruction::GetPointer() const
{
    return m_Pointer;
}

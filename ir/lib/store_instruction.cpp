#include <scc/ir/instruction.hpp>

#include <ostream>

scc::ir::StoreInstruction::StoreInstruction(
    Type *type,
    Block *block,
    Value *pointer,
    Value *value)
    : Instruction(type, block),
      m_Pointer(pointer),
      m_Value(value)
{
    m_Pointer->Use(this);
    m_Value->Use(this);
}

scc::ir::StoreInstruction::~StoreInstruction()
{
    DropAll();
}

void scc::ir::StoreInstruction::DropAll()
{
    if (m_Pointer)
    {
        m_Pointer->Drop(this);
        m_Pointer = nullptr;
    }
    if (m_Value)
    {
        m_Value->Drop(this);
        m_Value = nullptr;
    }
}

void scc::ir::StoreInstruction::Replace(Value *value, Value *with)
{
    if (m_Pointer == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Pointer = with;
    }

    if (m_Value == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Value = with;
    }
}

std::ostream &scc::ir::StoreInstruction::Print(std::ostream &stream) const
{
    return m_Value->PrintOperand(m_Pointer->PrintOperand(stream << "store ", true) << ", ", false);
}

scc::ir::Value *scc::ir::StoreInstruction::GetPointer() const
{
    return m_Pointer;
}

scc::ir::Value *scc::ir::StoreInstruction::GetValue() const
{
    return m_Value;
}

#include <scc/ir/instruction.hpp>

#include <ostream>

scc::ir::ReturnInstruction::ReturnInstruction(Type *type, Block *block)
    : Instruction(type, block),
      m_Value()
{
}

scc::ir::ReturnInstruction::ReturnInstruction(
    Type *type,
    Block *block,
    Value *value)
    : Instruction(type, block),
      m_Value(value)
{
    m_Value->Use(this);
}

scc::ir::ReturnInstruction::~ReturnInstruction()
{
    DropAll();
}

void scc::ir::ReturnInstruction::DropAll()
{
    if (m_Value)
    {
        m_Value->Drop(this);
        m_Value = nullptr;
    }
}

void scc::ir::ReturnInstruction::Replace(Value *value, Value *with)
{
    if (m_Value == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Value = with;
        return;
    }

    Instruction::Replace(value, with);
}

std::ostream &scc::ir::ReturnInstruction::Print(std::ostream &stream) const
{
    if (m_Value)
        return m_Value->PrintOperand(stream << "ret ");

    return stream << "ret void";
}

bool scc::ir::ReturnInstruction::IsTerminator() const
{
    return true;
}

scc::ir::Value *scc::ir::ReturnInstruction::GetValue() const
{
    return m_Value;
}

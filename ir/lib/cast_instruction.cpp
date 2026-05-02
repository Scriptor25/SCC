#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

#include <ostream>

scc::ir::CastInstruction::CastInstruction(
    Type *type,
    Block *block,
    std::string name,
    Value *value)
    : Instruction(type, block, std::move(name)),
      m_Value(value)
{
    m_Value->Use(this);
}

scc::ir::CastInstruction::~CastInstruction()
{
    DropAll();
}

void scc::ir::CastInstruction::DropAll()
{
    if (m_Value)
    {
        m_Value->Drop(this);
        m_Value = nullptr;
    }
}

void scc::ir::CastInstruction::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::CastInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    return m_Value->PrintOperand(m_Type->Print(stream) << " cast ");
}

scc::ir::Value *scc::ir::CastInstruction::GetValue() const
{
    return m_Value;
}

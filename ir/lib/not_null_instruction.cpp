#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>

scc::ir::NotNullInstruction::NotNullInstruction(Type *type, Block *block, std::string name, Value *value)
    : Instruction(type, block, std::move(name)),
      m_Value(value)
{
}

scc::ir::NotNullInstruction::~NotNullInstruction()
{
    DropAll();
}

void scc::ir::NotNullInstruction::DropAll()
{
    if (m_Value)
    {
        m_Value->Drop(this);
        m_Value = {};
    }
}

void scc::ir::NotNullInstruction::Replace(Value *value, Value *with)
{
    if (m_Value == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Value = with;
    }
}

std::ostream &scc::ir::NotNullInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    return m_Value->PrintOperand(stream << "notnull ", true);
}

std::ostream &scc::ir::NotNullInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
}

scc::ir::Value *scc::ir::NotNullInstruction::GetValue() const
{
    return m_Value;
}

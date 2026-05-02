#include <scc/ir/instruction.hpp>

#include <ostream>

scc::ir::CallInstruction::CallInstruction(
    Type *type,
    Block *block,
    std::string name,
    Value *callee,
    std::vector<Value *> arguments)
    : Instruction(type, block, std::move(name)),
      m_Callee(callee),
      m_Arguments(std::move(arguments))
{
    m_Callee->Use(this);

    for (const auto argument : m_Arguments)
        argument->Use(this);
}

scc::ir::CallInstruction::~CallInstruction()
{
    DropAll();
}

void scc::ir::CallInstruction::DropAll()
{
    if (m_Callee)
    {
        m_Callee->Drop(this);
        m_Callee = nullptr;
    }

    for (auto &argument : m_Arguments)
        if (argument)
        {
            argument->Drop(this);
            argument = nullptr;
        }
}

void scc::ir::CallInstruction::Replace(Value *value, Value *with)
{
    if (m_Callee == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Callee = with;
        return;
    }

    for (auto &argument : m_Arguments)
        if (argument == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            argument = with;
            return;
        }

    Instruction::Replace(value, with);
}

std::ostream &scc::ir::CallInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    m_Callee->PrintOperand(stream << "call ");

    for (const auto argument : m_Arguments)
        argument->PrintOperand(stream << ", ");

    return stream;
}

scc::ir::Value *scc::ir::CallInstruction::GetCallee() const
{
    return m_Callee;
}

unsigned scc::ir::CallInstruction::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::Value *scc::ir::CallInstruction::GetArgument(const unsigned index) const
{
    return m_Arguments[index];
}

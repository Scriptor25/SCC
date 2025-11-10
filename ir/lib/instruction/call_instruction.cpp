#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::CallInstruction::CallInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr callee,
    std::vector<ValueFwd::Ptr> arguments)
    : Instruction(std::move(type), std::move(register_), std::move(block)),
      m_Callee(std::move(callee)),
      m_Arguments(std::move(arguments))
{
    m_Callee->Use();
    for (const auto &argument : m_Arguments)
    {
        argument->Use();
    }
}

scc::ir::CallInstruction::~CallInstruction()
{
    m_Callee->Drop();
    for (const auto &argument : m_Arguments)
    {
        argument->Drop();
    }
}

std::ostream &scc::ir::CallInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
    {
        m_Register->Print(stream) << " = ";
    }
    m_Callee->PrintOperand(stream << "call ");
    for (auto &argument : m_Arguments)
    {
        argument->PrintOperand(stream << ", ");
    }
    return stream;
}

scc::ir::ValueFwd::Ptr scc::ir::CallInstruction::GetCallee() const
{
    return m_Callee;
}

unsigned scc::ir::CallInstruction::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::ValueFwd::Ptr scc::ir::CallInstruction::GetArgument(const unsigned index) const
{
    return m_Arguments.at(index);
}

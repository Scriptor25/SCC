#include <scc/ir/instruction.hpp>
#include <scc/ir/register.hpp>

scc::ir::CallInstruction::CallInstruction(
    TypeFwd::Ptr type,
    RegisterFwd::Ptr register_,
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr callee,
    std::vector<ValueFwd::Ptr> arguments)
    : IdentifiedInstruction(std::move(type), std::move(register_), std::move(block)),
      m_Callee(std::move(callee)),
      m_Arguments(std::move(arguments))
{
}

std::ostream &scc::ir::CallInstruction::Print(std::ostream &stream) const
{
    m_Callee->PrintOperand(m_Register->Print(stream) << " = call ");
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

scc::ir::CallVoidInstruction::CallVoidInstruction(
    BlockFwd::WeakPtr block,
    ValueFwd::Ptr callee,
    std::vector<ValueFwd::Ptr> arguments)
    : Instruction(std::move(block)),
      m_Callee(std::move(callee)),
      m_Arguments(std::move(arguments))
{
}

std::ostream &scc::ir::CallVoidInstruction::Print(std::ostream &stream) const
{
    m_Callee->PrintOperand(stream << "call ");
    for (auto &argument : m_Arguments)
    {
        argument->PrintOperand(stream << ", ");
    }
    return stream;
}

scc::ir::ValueFwd::Ptr scc::ir::CallVoidInstruction::GetCallee() const
{
    return m_Callee;
}

unsigned scc::ir::CallVoidInstruction::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::ValueFwd::Ptr scc::ir::CallVoidInstruction::GetArgument(const unsigned index) const
{
    return m_Arguments.at(index);
}

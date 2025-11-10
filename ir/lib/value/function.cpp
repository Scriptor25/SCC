#include <scc/ir/block.hpp>
#include <scc/ir/register.hpp>
#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::Function::Function(const FunctionType::Ptr &type, std::string name)
    : Global(type, std::move(name))
{
    m_Result = type->GetResult();
    m_Variadic = type->IsVariadic();
    for (auto &argument : *type)
    {
        m_Arguments.emplace_back(std::make_shared<Argument>(argument, CreateRegister()));
    }
}

std::ostream &scc::ir::Function::Print(std::ostream &stream) const
{
    m_Result->Print(stream << "function ") << " @" << m_Name << '(';
    for (auto i = m_Arguments.begin(); i != m_Arguments.end(); ++i)
    {
        if (i != m_Arguments.begin())
        {
            stream << ", ";
        }
        (*i)->Print(stream);
    }
    if (m_Variadic)
    {
        if (!m_Arguments.empty())
        {
            stream << ", ";
        }
        stream << "...";
    }
    stream << ')';

    if (m_Blocks.empty())
    {
        return stream;
    }

    stream << " {" << std::endl;

    for (auto &block : m_Blocks)
    {
        block->Print(stream) << std::endl;
    }

    return stream << '}';
}

void scc::ir::Function::Insert(BlockFwd::Ptr block)
{
    m_Blocks.emplace_back(std::move(block));
}

unsigned scc::ir::Function::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::Argument::Ptr scc::ir::Function::GetArgument(const unsigned index) const
{
    return m_Arguments.at(index);
}

scc::ir::RegisterFwd::Ptr scc::ir::Function::CreateRegister(std::string name)
{
    if (name.empty())
    {
        name = std::to_string(m_Registers.size());
    }
    return m_Registers.emplace_back(std::make_shared<Register>(std::move(name)));
}

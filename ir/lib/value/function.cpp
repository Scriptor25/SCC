#include <scc/ir/block.hpp>
#include <scc/ir/register.hpp>
#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

#include <scc/assert.hpp>

#include <ostream>

scc::ir::Function::Function(const FunctionType::Ptr &type, std::string name)
    : Global(type, std::move(name))
{
    m_Result = type->GetResult();
    m_Variadic = type->IsVariadic();
    for (auto &argument : *type)
    {
        auto register_ = CreateRegister();
        auto value = m_Arguments.emplace_back(std::make_shared<Argument>(argument, register_));
        register_->SetValue(value);
    }
}

std::ostream &scc::ir::Function::Print(std::ostream &stream) const
{
    m_Result->Print(stream << "function ") << " @" << m_Name << '(';
    for (auto i = m_Arguments.begin(); i != m_Arguments.end(); ++i)
    {
        if (i != m_Arguments.begin())
            stream << ", ";
        (*i)->Print(stream);
    }
    if (m_Variadic)
    {
        if (!m_Arguments.empty())
            stream << ", ";
        stream << "...";
    }
    stream << ')';

    if (m_Blocks.empty())
        return stream;

    stream << " {" << std::endl;

    for (auto &block : m_Blocks)
        block->Print(stream) << std::endl;

    return stream << '}';
}

void scc::ir::Function::Insert(BlockFwd::Ptr block)
{
    for (const auto &entry : m_Blocks)
        Assert(entry != block, "duplicate insert block");
    m_Blocks.emplace_back(std::move(block));
}

scc::ir::BlockFwd::Ptr scc::ir::Function::Find(const std::string &name) const
{
    for (const auto &block : m_Blocks)
        if (block->GetName() == name)
            return block;
    return nullptr;
}

unsigned scc::ir::Function::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::Argument::Ptr scc::ir::Function::GetArgument(const unsigned index) const
{
    Assert(index < m_Arguments.size(), "index out of bounds");
    return m_Arguments.at(index);
}

scc::ir::RegisterFwd::Ptr scc::ir::Function::CreateRegister(std::string name)
{
    if (name.empty())
        name = std::to_string(m_Registers.size());
    return m_Registers.emplace_back(std::make_shared<Register>(std::move(name)));
}

scc::ir::RegisterFwd::Ptr scc::ir::Function::FindRegister(const std::string &name) const
{
    for (const auto &register_ : m_Registers)
        if (register_->GetName() == name)
            return register_;
    return nullptr;
}

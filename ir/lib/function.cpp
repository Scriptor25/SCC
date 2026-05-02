#include <scc/ir/argument.hpp>
#include <scc/ir/block.hpp>
#include <scc/ir/function.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::Function::Function(FunctionType *type, std::string name)
    : Global(type, std::move(name))
{
    m_Result = type->GetResult();
    m_Variadic = type->IsVariadic();

    for (const auto argument : *type)
        m_Arguments.push_back(std::make_unique<Argument>(argument));
}

scc::ir::Function::~Function()
{
    for (const auto &block : m_Blocks)
        block->ReplaceWith(nullptr);

    for (const auto &argument : m_Arguments)
        argument->ReplaceWith(nullptr);
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

void scc::ir::Function::InsertBlock(std::unique_ptr<Block> block)
{
    Assert(!!block, "block must not be null");

    for (auto &entry : m_Blocks)
        Assert(entry != block, "duplicate insert block");

    m_Blocks.push_back(std::move(block));
}

scc::ir::Block *scc::ir::Function::CreateBlock(std::string name)
{
    auto block = std::make_unique<Block>(std::move(name), this);
    auto *ptr = block.get();

    m_Blocks.push_back(std::move(block));

    return ptr;
}

scc::ir::Block *scc::ir::Function::FindBlock(const std::string &name) const
{
    for (auto &block : m_Blocks)
        if (block->GetName() == name)
            return block.get();

    return nullptr;
}

scc::ir::Value *scc::ir::Function::FindValue(const std::string &name) const
{
    for (auto &argument : m_Arguments)
        if (argument->GetName() == name)
            return argument.get();

    for (auto &block : m_Blocks)
    {
        if (block->GetName() == name)
            return block.get();

        if (auto value = block->FindValue(name))
            return value;
    }

    return nullptr;
}

unsigned scc::ir::Function::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::Argument *scc::ir::Function::GetArgument(const unsigned index) const
{
    AssertIndexInBounds(index, m_Arguments.size());

    return m_Arguments[index].get();
}

std::unordered_set<scc::ir::Block *> scc::ir::Function::GetPredecessors(const Block *block) const
{
    std::unordered_set<Block *> predecessors;

    for (auto &entry : m_Blocks)
        if (entry->GetSuccessors().contains(const_cast<Block *>(block)))
            predecessors.insert(entry.get());

    return predecessors;
}

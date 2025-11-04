#include <scc/assert.hpp>
#include <scc/ir/module.hpp>

bool scc::ir::Module::HasSymbol(const std::string &name) const
{
    return m_Symbols.contains(name);
}

scc::ir::GlobalPtr scc::ir::Module::GetSymbol(const std::string &name) const
{
    Assert(m_Symbols.contains(name), "symbol {} does not exist", name);
    return m_Symbols.at(name);
}

scc::ir::Variable::Ptr scc::ir::Module::CreateVariable(TypePtr type, const std::string &name)
{
    Assert(!m_Symbols.contains(name), "variable {} does already exist", name);

    auto value = std::make_shared<Variable>(std::move(type), name);
    m_Symbols.emplace(name, value);
    return value;
}

scc::ir::Function::Ptr scc::ir::Module::CreateFunction(TypePtr type, const std::string &name)
{
    Assert(!m_Symbols.contains(name), "function {} does already exist", name);

    auto value = std::make_shared<Function>(std::move(type), name);
    m_Symbols.emplace(name, value);
    return value;
}

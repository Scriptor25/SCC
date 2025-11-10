#include <ranges>
#include <scc/error.hpp>
#include <scc/ir/module.hpp>

void scc::ir::Module::SetName(std::string name)
{
    m_Name = std::move(name);
}

std::string scc::ir::Module::GetName()
{
    return m_Name;
}

bool scc::ir::Module::HasSymbol(const std::string &name) const
{
    for (auto &symbol : m_Symbols)
    {
        if (symbol->GetName() == name)
        {
            return true;
        }
    }
    return false;
}

scc::ir::GlobalFwd::Ptr scc::ir::Module::GetSymbol(const std::string &name) const
{
    for (auto &symbol : m_Symbols)
    {
        if (symbol->GetName() == name)
        {
            return symbol;
        }
    }
    Error("symbol {} does not exist", name);
}

scc::ir::Variable::Ptr scc::ir::Module::CreateVariable(
    TypeFwd::Ptr type,
    std::string name,
    ConstantFwd::Ptr initializer)
{
    for (const auto &symbol : m_Symbols)
    {
        if (symbol->GetName() == name)
        {
            Error("variable {} does already exist", name);
        }
    }

    auto value = std::make_shared<Variable>(std::move(type), std::move(name), std::move(initializer));
    m_Symbols.emplace_back(value);
    return value;
}

scc::ir::Function::Ptr scc::ir::Module::CreateFunction(FunctionType::Ptr type, std::string name)
{
    for (const auto &symbol : m_Symbols)
    {
        if (symbol->GetName() == name)
        {
            Error("function {} does already exist", name);
        }
    }

    auto value = std::make_shared<Function>(std::move(type), std::move(name));
    m_Symbols.emplace_back(value);
    return value;
}

std::ostream &scc::ir::Module::Print(std::ostream &stream) const
{
    for (auto &symbol : m_Symbols)
    {
        symbol->Print(stream) << std::endl;
    }
    return stream;
}

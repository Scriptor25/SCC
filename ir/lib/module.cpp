#include <scc/ir/function.hpp>
#include <scc/ir/module.hpp>
#include <scc/ir/variable.hpp>

#include <scc/error.hpp>

#include <ostream>
#include <ranges>

scc::ir::Module::~Module()
{
    for (const auto &symbol : m_Symbols)
    {
        symbol->ReplaceWith(nullptr);
        symbol->DropAll();
    }
}

scc::ir::Module::Module(Module &&module) noexcept
    : m_Name(std::move(module.m_Name)),
      m_Symbols(std::move(module.m_Symbols))
{
}

scc::ir::Module &scc::ir::Module::operator=(Module &&module) noexcept
{
    std::swap(m_Name, module.m_Name);
    std::swap(m_Symbols, module.m_Symbols);

    return *this;
}

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
        if (symbol->GetName() == name)
            return true;

    return false;
}

scc::ir::Global *scc::ir::Module::GetSymbol(const std::string &name) const
{
    for (auto &symbol : m_Symbols)
        if (symbol->GetName() == name)
            return symbol.get();

    Error("symbol {} does not exist", name);
}

scc::ir::Variable *scc::ir::Module::CreateVariable(
    Type *type,
    std::string name,
    Constant *initializer)
{
    for (const auto &symbol : m_Symbols)
        if (symbol->GetName() == name)
            Error("variable {} does already exist", name);

    auto value = std::make_unique<Variable>(type, std::move(name), initializer);
    auto *ptr = value.get();

    m_Symbols.push_back(std::move(value));

    return ptr;
}

scc::ir::Function *scc::ir::Module::CreateFunction(FunctionType *type, std::string name)
{
    for (const auto &symbol : m_Symbols)
        if (symbol->GetName() == name)
            Error("function {} does already exist", name);

    auto value = std::make_unique<Function>(type, std::move(name));
    auto *ptr = value.get();

    m_Symbols.push_back(std::move(value));

    return ptr;
}

std::ostream &scc::ir::Module::Print(std::ostream &stream) const
{
    for (auto &symbol : m_Symbols)
        symbol->Print(stream) << std::endl;

    return stream;
}

#include <scc/as/module.hpp>

scc::as::Section *scc::as::Module::CreateSection(std::string name)
{
    m_Sections.push_back(std::make_unique<Section>(this, std::move(name)));

    return m_Sections.back().get();
}

scc::as::Symbol *scc::as::Module::CreateSymbol(std::string name)
{
    m_Symbols.push_back(std::make_unique<Symbol>(std::move(name)));

    return m_Symbols.back().get();
}

scc::as::Symbol *scc::as::Module::CreateSymbol(Fragment &fragment, std::string name)
{
    m_Symbols.push_back(std::make_unique<Symbol>(fragment, std::move(name)));

    return m_Symbols.back().get();
}

scc::as::Symbol *scc::as::Module::CreateSymbol(Section &section, const size_t index, std::string name)
{
    m_Symbols.push_back(std::make_unique<Symbol>(section, index, std::move(name)));

    return m_Symbols.back().get();
}

scc::as::Section *scc::as::Module::GetSection(const std::string &name) const
{
    for (auto &section : m_Sections)
        if (section->GetName() == name)
            return section.get();

    return nullptr;
}

scc::as::Symbol *scc::as::Module::GetSymbol(const std::string &name) const
{
    for (auto &symbol : m_Symbols)
        if (symbol->GetName() == name)
            return symbol.get();

    return nullptr;
}

scc::as::Symbol *scc::as::Module::GetSymbol(Fragment *fragment) const
{
    for (auto &symbol : m_Symbols)
        if (symbol->GetFragment() == fragment)
            return symbol.get();

    return nullptr;
}

scc::as::Section *scc::as::Module::GetOrCreateSection(const std::string &name)
{
    if (auto *ptr = GetSection(name))
        return ptr;

    return CreateSection(name);
}

scc::as::Symbol *scc::as::Module::GetOrCreateSymbol(const std::string &name)
{
    if (auto *ptr = GetSymbol(name))
        return ptr;

    return CreateSymbol(name);
}

std::ostream &scc::as::Module::Print(std::ostream &stream) const
{
    for (auto &section : m_Sections)
        section->Print(stream);

    return stream;
}

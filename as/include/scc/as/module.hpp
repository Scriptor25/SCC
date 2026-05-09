#pragma once

#include <scc/as/as.hpp>
#include <scc/as/section.hpp>
#include <scc/as/symbol.hpp>

#include <memory>
#include <vector>

namespace scc::as
{
    class Module
    {
    public:
        Section *CreateSection(std::string name = {});

        Symbol *CreateSymbol(std::string name = {});
        Symbol *CreateSymbol(Fragment &fragment, std::string name = {});
        Symbol *CreateSymbol(Section &section, size_t index, std::string name = {});

        [[nodiscard]] Section *GetSection(const std::string &name) const;
        [[nodiscard]] Symbol *GetSymbol(const std::string &name) const;

        Section *GetOrCreateSection(const std::string &name);

    private:
        std::vector<std::unique_ptr<Section>> m_Sections;
        std::vector<std::unique_ptr<Symbol>> m_Symbols;
    };
}

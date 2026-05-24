#pragma once

#include <scc/as/as.hpp>
#include <scc/as/section.hpp>
#include <scc/as/symbol.hpp>

#include <scc/common.hpp>

#include <iosfwd>
#include <memory>
#include <vector>

namespace scc::as
{
    class Module
    {
    public:
        explicit Module(const Platform &platform);

        [[nodiscard]] const Platform &GetPlatform() const;

        Section *CreateSection(std::string name = {});

        Symbol *CreateSymbol(std::string name = {});
        Symbol *CreateSymbol(Section &section, size_t index, std::string name = {});

        [[nodiscard]] Section *GetSection(const std::string &name) const;

        [[nodiscard]] Symbol *GetSymbol(const std::string &name) const;
        [[nodiscard]] std::vector<Symbol *> GetSymbols(const Section &section, size_t index) const;
        [[nodiscard]] std::vector<Symbol *> GetSymbols(const Fragment &fragment) const;

        Section *GetOrCreateSection(const std::string &name);
        Symbol *GetOrCreateSymbol(const std::string &name);

        std::ostream &Print(std::ostream &stream) const;
        void Encode(std::vector<uint8_t> &buffer) const;

    private:
        const Platform &m_Platform;

        std::vector<std::unique_ptr<Section>> m_Sections;
        std::vector<std::unique_ptr<Symbol>> m_Symbols;
    };
}

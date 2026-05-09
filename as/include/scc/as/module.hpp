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

    private:
        std::vector<std::unique_ptr<Section>> m_Sections;
        std::vector<std::unique_ptr<Symbol>> m_Symbols;
    };
}

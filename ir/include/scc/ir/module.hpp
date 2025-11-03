#pragma once

#include <unordered_map>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Module final
    {
    public:
        Global::Ptr GetSymbol(const std::string &name) const;

        Variable::Ptr CreateVariable(const std::string &name) const;
        Function::Ptr CreateFunction(const std::string &name) const;

    private:
        std::unordered_map<std::string, Global::Ptr> m_Symbols;
    };
}

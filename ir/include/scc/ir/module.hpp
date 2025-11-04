#pragma once

#include <unordered_map>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Module final
    {
    public:
        [[nodiscard]] bool HasSymbol(const std::string &name) const;
        [[nodiscard]] GlobalPtr GetSymbol(const std::string &name) const;

        Variable::Ptr CreateVariable(TypePtr type, const std::string &name);
        Function::Ptr CreateFunction(TypePtr type, const std::string &name);

    private:
        std::unordered_map<std::string, GlobalPtr> m_Symbols;
    };
}

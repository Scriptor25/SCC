#pragma once

#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Module final
    {
    public:
        void SetName(std::string name);
        std::string GetName();

        [[nodiscard]] bool HasSymbol(const std::string &name) const;
        [[nodiscard]] GlobalPtr GetSymbol(const std::string &name) const;

        Variable::Ptr CreateVariable(TypePtr type, std::string name, ConstantPtr initializer);
        Function::Ptr CreateFunction(FunctionType::Ptr type, std::string name);

        std::ostream &Print(std::ostream &stream) const;

    private:
        std::string m_Name;
        std::vector<GlobalPtr> m_Symbols;
    };
}

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
        [[nodiscard]] GlobalFwd::Ptr GetSymbol(const std::string &name) const;

        Variable::Ptr CreateVariable(TypeFwd::Ptr type, std::string name, ConstantFwd::Ptr initializer);
        Function::Ptr CreateFunction(FunctionType::Ptr type, std::string name);

        std::ostream &Print(std::ostream &stream) const;

    private:
        std::string m_Name;
        std::vector<GlobalFwd::Ptr> m_Symbols;
    };
}

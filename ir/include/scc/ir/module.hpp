#pragma once

#include <scc/ir/global.hpp>
#include <scc/ir/ir.hpp>

#include <memory>
#include <string>
#include <vector>

namespace scc::ir
{
    class Module
    {
    public:
        Module() = default;
        ~Module();

        Module(const Module &) = delete;
        Module(Module &&module) noexcept;

        Module &operator=(const Module &) = delete;
        Module &operator=(Module &&module) noexcept;

        void SetName(std::string name);
        std::string GetName();

        [[nodiscard]] Global *GetSymbol(const std::string &name) const;
        Global *GetOrCreateSymbol(Type *type, std::string name);

        Variable *CreateVariable(Type *type, std::string name, Constant *initializer);
        Function *CreateFunction(FunctionType *type, std::string name);

        std::ostream &Print(std::ostream &stream) const;
        std::ostream &PrintAssembly(std::ostream &stream, const Platform &platform) const;

    private:
        std::string m_Name;
        std::vector<std::unique_ptr<Global>> m_Symbols;
    };
}

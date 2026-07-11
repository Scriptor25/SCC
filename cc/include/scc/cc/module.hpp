#pragma once

#include <scc/ir/ir.hpp>

namespace scc::cc
{
    class Module
    {
    public:
        explicit Module(ir::Module &ir_module);

        [[nodiscard]] ir::Module &GetIRModule() const;

    private:
        ir::Module &m_IRModule;
    };
}

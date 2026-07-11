#include <scc/cc/module.hpp>

scc::cc::Module::Module(ir::Module &ir_module)
    : m_IRModule(ir_module)
{
}

scc::ir::Module &scc::cc::Module::GetIRModule() const
{
    return m_IRModule;
}

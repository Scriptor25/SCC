#include <scc/ir/value.hpp>

scc::ir::Function::Function(TypePtr type, std::string name)
    : Global(std::move(type), std::move(name))
{
}

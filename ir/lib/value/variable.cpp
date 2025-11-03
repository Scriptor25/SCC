#include <scc/ir/value.hpp>

scc::ir::Variable::Variable(TypePtr type, std::string name)
    : Global(std::move(type), std::move(name))
{
}

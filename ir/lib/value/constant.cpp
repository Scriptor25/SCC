#include <scc/ir/value.hpp>

scc::ir::Constant::Constant(TypePtr type)
    : Value(std::move(type))
{
}

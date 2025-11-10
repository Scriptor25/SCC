#include <scc/ir/value.hpp>

scc::ir::Constant::Constant(TypeFwd::Ptr type)
    : Value(std::move(type))
{
}

std::ostream &scc::ir::Constant::Print(std::ostream &stream) const
{
    return PrintOperand(stream);
}

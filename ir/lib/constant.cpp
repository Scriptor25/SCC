#include <scc/ir/constant.hpp>

scc::ir::Constant::Constant(Type *type)
    : User(type)
{
}

std::ostream &scc::ir::Constant::Print(std::ostream &stream) const
{
    return PrintOperand(stream);
}

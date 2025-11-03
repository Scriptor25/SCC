#include <scc/ir/value.hpp>

scc::ir::ConstantFloat::ConstantFloat(TypePtr type, const double value)
    : Constant(std::move(type)),
      m_Value(value)
{
}

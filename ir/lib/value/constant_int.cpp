#include <scc/ir/value.hpp>

scc::ir::ConstantInt::ConstantInt(TypePtr type, const uint64_t value)
    : Constant(std::move(type)),
      m_Value(value)
{
}

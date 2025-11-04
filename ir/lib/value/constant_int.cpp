#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantInt::ConstantInt(TypePtr type, const uint64_t value)
    : Constant(std::move(type)),
      m_Value(value)
{
}

std::ostream &scc::ir::ConstantInt::Print(std::ostream &stream) const
{
    return m_Type->Print(stream) << " const " << std::hex << m_Value << std::dec;
}

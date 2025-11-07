#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantInt::ConstantInt(IntType::Ptr type, const uint64_t value)
    : Constant(std::move(type)),
      m_Value(value)
{
}

std::ostream &scc::ir::ConstantInt::PrintOperand(std::ostream &stream) const
{
    return m_Type->Print(stream) << " 0x" << std::hex << m_Value << std::dec;
}

uint64_t scc::ir::ConstantInt::GetValue() const
{
    return m_Value;
}

#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

scc::ir::ConstantInt::ConstantInt(IntType *type, const uint64_t value)
    : Constant(type),
      m_Value(value)
{
}

std::ostream &scc::ir::ConstantInt::PrintOperand(std::ostream &stream, const bool print_type) const
{
    if (print_type)
        m_Type->Print(stream) << ' ';

    return stream << "0x" << std::hex << m_Value;
}

bool scc::ir::ConstantInt::Compare(Constant *value) const
{
    if (const auto *int_value = dynamic_cast<ConstantInt *>(value))
        return m_Type == int_value->m_Type && m_Value == int_value->m_Value;

    return false;
}

uint64_t scc::ir::ConstantInt::GetValue() const
{
    return m_Value;
}

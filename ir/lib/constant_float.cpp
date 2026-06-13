#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ConstantFloat::ConstantFloat(FloatType *type, const float64_t value)
    : Constant(type),
      m_Value(value)
{
}

std::ostream &scc::ir::ConstantFloat::PrintOperand(std::ostream &stream, const bool print_type) const
{
    if (print_type)
        m_Type->Print(stream) << ' ';

    switch (dynamic_cast<FloatType *>(m_Type)->GetBitWidth())
    {
    case 32:
    {
        const auto float_value = static_cast<float32_t>(m_Value);
        return stream << "0x" << std::hex << reinterpret_cast<const uint32_t &>(float_value);
    }

    case 64:
        return stream << "0x" << std::hex << reinterpret_cast<const uint64_t &>(m_Value);

    default:
        return stream << "NaN";
    }
}

std::ostream &scc::ir::ConstantFloat::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
}

std::ostream &scc::ir::ConstantFloat::PrintOperandAssembly(
    std::ostream &stream,
    LoweringContext &context,
    bool address) const
{
    Error("TODO");
}

bool scc::ir::ConstantFloat::Compare(Constant *value) const
{
    if (const auto *float_value = dynamic_cast<ConstantFloat *>(value))
        return m_Type == float_value->m_Type && m_Value == float_value->m_Value;

    return false;
}

scc::ir::float64_t scc::ir::ConstantFloat::GetValue() const
{
    return m_Value;
}

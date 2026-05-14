#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

#include <scc/platform.hpp>

scc::ir::IntType::IntType(Context &context, const size_t bit_width)
    : Type(context, Kind::Int),
      m_BitWidth(bit_width)
{
}

bool scc::ir::IntType::Compare(Type *type) const
{
    if (const auto *int_type = dynamic_cast<IntType *>(type))
        return m_BitWidth == int_type->m_BitWidth;

    return false;
}

size_t scc::ir::IntType::GetSize() const
{
    return (m_BitWidth + 0b111) >> 3;
}

size_t scc::ir::IntType::GetAlign() const
{
    return std::min(GetSize(), m_Context.GetPlatform().ABI.DataLayout.MaxIntAlign);
}

bool scc::ir::IntType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::IntType::Print(std::ostream &stream) const
{
    return stream << 'i' << std::dec << m_BitWidth;
}

size_t scc::ir::IntType::GetBitWidth() const
{
    return m_BitWidth;
}

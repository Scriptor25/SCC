#include <scc/ir/context.hpp>
#include <scc/ir/platform.hpp>
#include <scc/ir/type.hpp>

scc::ir::FloatType::FloatType(Context &context, const unsigned bit_width)
    : Type(context, Kind::Float),
      m_BitWidth(bit_width)
{
}

bool scc::ir::FloatType::Compare(Type *type) const
{
    if (const auto p = dynamic_cast<FloatType *>(type))
        return m_BitWidth == p->m_BitWidth;

    return false;
}

size_t scc::ir::FloatType::GetSize() const
{
    return (m_BitWidth + 0b111) >> 3;
}

size_t scc::ir::FloatType::GetAlign() const
{
    return std::min(GetSize(), m_Context.GetPlatform().MaxFloatAlign);
}

bool scc::ir::FloatType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::FloatType::Print(std::ostream &stream) const
{
    return stream << "f" << m_BitWidth;
}

unsigned scc::ir::FloatType::GetBitWidth() const
{
    return m_BitWidth;
}

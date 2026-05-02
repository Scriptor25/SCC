#include <scc/ir/context.hpp>
#include <scc/ir/platform.hpp>
#include <scc/ir/type.hpp>

scc::ir::FloatType::FloatType(Context &context, const unsigned size_bits)
    : Type(context, Kind::Float),
      m_SizeBits(size_bits)
{
}

bool scc::ir::FloatType::Compare(Type *type) const
{
    if (const auto p = dynamic_cast<FloatType *>(type))
        return m_SizeBits == p->m_SizeBits;

    return false;
}

unsigned scc::ir::FloatType::GetSize() const
{
    return (m_SizeBits + 0b111) >> 3;
}

unsigned scc::ir::FloatType::GetAlign() const
{
    return std::min(GetSize(), m_Context.GetPlatform().PointerSize);
}

bool scc::ir::FloatType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::FloatType::Print(std::ostream &stream) const
{
    return stream << "f" << m_SizeBits;
}

unsigned scc::ir::FloatType::GetSizeBits() const
{
    return m_SizeBits;
}

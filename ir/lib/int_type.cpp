#include <scc/ir/context.hpp>
#include <scc/ir/platform.hpp>
#include <scc/ir/type.hpp>

scc::ir::IntType::IntType(Context &context, const unsigned size_bits)
    : Type(context, Kind::Int),
      m_SizeBits(size_bits)
{
}

bool scc::ir::IntType::Compare(Type *type) const
{
    if (const auto p = dynamic_cast<IntType *>(type))
        return m_SizeBits == p->m_SizeBits;

    return false;
}

unsigned scc::ir::IntType::GetSize() const
{
    return (m_SizeBits + 0b111) >> 3;
}

unsigned scc::ir::IntType::GetAlign() const
{
    return std::min(GetSize(), m_Context.GetPlatform().PointerSize);
}

bool scc::ir::IntType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::IntType::Print(std::ostream &stream) const
{
    return stream << 'i' << m_SizeBits;
}

unsigned scc::ir::IntType::GetSizeBits() const
{
    return m_SizeBits;
}

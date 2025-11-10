#include <scc/ir/type.hpp>

scc::ir::FloatType::FloatType(Context &context, const unsigned size_bits)
    : Type(context, Kind_Float),
      m_SizeBits(size_bits)
{
}

unsigned scc::ir::FloatType::GenerateHash() const
{
    return CombineHash(2, m_SizeBits);
}

bool scc::ir::FloatType::Equals(const TypeFwd::Ptr &type) const
{
    if (type->GetKind() != Kind_Float)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<FloatType>(type))
    {
        return m_SizeBits == p->m_SizeBits;
    }

    return false;
}

std::ostream &scc::ir::FloatType::Print(std::ostream &stream) const
{
    return stream << "f" << m_SizeBits;
}

unsigned scc::ir::FloatType::GetSizeBits() const
{
    return m_SizeBits;
}

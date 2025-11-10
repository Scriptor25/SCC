#include <scc/ir/type.hpp>

scc::ir::IntType::IntType(Context &context, const unsigned size_bits)
    : Type(context, Kind_Int),
      m_SizeBits(size_bits)
{
}

unsigned scc::ir::IntType::GenerateHash() const
{
    return CombineHash(1, m_SizeBits);
}

bool scc::ir::IntType::Equals(const TypeFwd::Ptr &type) const
{
    if (type->GetKind() != Kind_Int)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<IntType>(type))
    {
        return m_SizeBits == p->m_SizeBits;
    }

    return false;
}

std::ostream &scc::ir::IntType::Print(std::ostream &stream) const
{
    return stream << "i" << m_SizeBits;
}

unsigned scc::ir::IntType::GetSizeBits() const
{
    return m_SizeBits;
}

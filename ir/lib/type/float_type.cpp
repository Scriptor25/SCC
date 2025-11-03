#include <scc/ir/type.hpp>

scc::ir::FloatType::FloatType(Context &context, const unsigned size_bytes)
    : Type(context, Kind_Float),
      m_SizeBytes(size_bytes)
{
}

unsigned scc::ir::FloatType::GenerateHash() const
{
    return CombineHash(2, m_SizeBytes);
}

bool scc::ir::FloatType::Equals(const TypePtr &type) const
{
    if (type->GetKind() != Kind_Float)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<FloatType>(type))
    {
        return m_SizeBytes == p->m_SizeBytes;
    }

    return false;
}

unsigned scc::ir::FloatType::GetSizeBytes() const
{
    return m_SizeBytes;
}

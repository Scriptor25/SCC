#include <scc/ir/type.hpp>

scc::ir::IntType::IntType(Context &context, const unsigned size_bytes)
    : Type(context, Kind_Int),
      m_SizeBytes(size_bytes)
{
}

unsigned scc::ir::IntType::GenerateHash() const
{
    return CombineHash(1, m_SizeBytes);
}

bool scc::ir::IntType::Equals(const TypePtr &type) const
{
    if (type->GetKind() != Kind_Int)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<IntType>(type))
    {
        return m_SizeBytes == p->m_SizeBytes;
    }

    return false;
}

std::ostream &scc::ir::IntType::Print(std::ostream &stream) const
{
    return stream << "i" << (m_SizeBytes * 8u);
}

unsigned scc::ir::IntType::GetSizeBytes() const
{
    return m_SizeBytes;
}

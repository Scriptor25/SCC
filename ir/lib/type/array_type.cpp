#include <scc/ir/type.hpp>

scc::ir::ArrayType::ArrayType(Context &context, TypePtr base, const unsigned length)
    : Type(context, Kind_Array),
      m_Base(std::move(base)),
      m_Length(length)
{
}

unsigned scc::ir::ArrayType::GenerateHash() const
{
    return CombineHash(4, CombineHash(m_Base->GenerateHash(), m_Length));
}

bool scc::ir::ArrayType::Equals(const TypePtr &type) const
{
    if (type->GetKind() != Kind_Array)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<ArrayType>(type))
    {
        return m_Base == p->m_Base && m_Length == p->m_Length;
    }

    return false;
}

scc::ir::TypePtr scc::ir::ArrayType::GetBase() const
{
    return m_Base;
}

unsigned scc::ir::ArrayType::GetLength() const
{
    return m_Length;
}

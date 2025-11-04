#include <scc/ir/type.hpp>

scc::ir::VectorType::VectorType(Context &context, TypePtr base, const unsigned length)
    : Type(context, Kind_Vector),
      m_Base(std::move(base)),
      m_Length(length)
{
}

unsigned scc::ir::VectorType::GenerateHash() const
{
    return CombineHash(5, CombineHash(m_Base->GenerateHash(), m_Length));
}

bool scc::ir::VectorType::Equals(const TypePtr &type) const
{
    if (type->GetKind() != Kind_Vector)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<VectorType>(type))
    {
        return m_Base == p->m_Base && m_Length == p->m_Length;
    }

    return false;
}

std::ostream &scc::ir::VectorType::Print(std::ostream &stream) const
{
    return m_Base->Print(stream << '<' << m_Length << " x ") << '>';
}

scc::ir::TypePtr scc::ir::VectorType::GetBase() const
{
    return m_Base;
}

unsigned scc::ir::VectorType::GetLength() const
{
    return m_Length;
}

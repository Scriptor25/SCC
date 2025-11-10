#include <scc/assert.hpp>
#include <scc/ir/type.hpp>

scc::ir::VectorType::VectorType(Context &context, TypeFwd::Ptr base, const unsigned length)
    : Type(context, Kind_Vector),
      m_Base(std::move(base)),
      m_Length(length)
{
}

unsigned scc::ir::VectorType::GenerateHash() const
{
    return CombineHash(5, CombineHash(m_Base->GenerateHash(), m_Length));
}

bool scc::ir::VectorType::Equals(const TypeFwd::Ptr &type) const
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

scc::ir::TypeFwd::Ptr scc::ir::VectorType::GetBase() const
{
    return m_Base;
}

unsigned scc::ir::VectorType::GetLength() const
{
    return m_Length;
}

unsigned scc::ir::VectorType::GetElementCount() const
{
    return m_Length;
}

scc::ir::Shared<scc::ir::Type>::Ptr scc::ir::VectorType::GetElement(const unsigned index) const
{
    Assert(index < m_Length, "element index out of bounds");
    return m_Base;
}

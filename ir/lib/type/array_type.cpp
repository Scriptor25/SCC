#include <scc/assert.hpp>
#include <scc/ir/type.hpp>

scc::ir::ArrayType::ArrayType(Context &context, TypeFwd::Ptr base, const unsigned length)
    : Type(context, Kind_Array),
      m_Base(std::move(base)),
      m_Length(length)
{
}

unsigned scc::ir::ArrayType::GenerateHash() const
{
    return CombineHash(4, CombineHash(m_Base->GenerateHash(), m_Length));
}

bool scc::ir::ArrayType::Equals(const TypeFwd::Ptr &type) const
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

std::ostream &scc::ir::ArrayType::Print(std::ostream &stream) const
{
    return m_Base->Print(stream << '[' << m_Length << " x ") << ']';
}

scc::ir::TypeFwd::Ptr scc::ir::ArrayType::GetBase() const
{
    return m_Base;
}

unsigned scc::ir::ArrayType::GetLength() const
{
    return m_Length;
}

unsigned scc::ir::ArrayType::GetElementCount() const
{
    return m_Length;
}

scc::ir::Shared<scc::ir::Type>::Ptr scc::ir::ArrayType::GetElement(const unsigned index) const
{
    Assert(index < m_Length, "element index out of bounds");
    return m_Base;
}

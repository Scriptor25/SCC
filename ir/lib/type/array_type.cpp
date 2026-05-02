#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ArrayType::ArrayType(Context &context, TypeFwd::Ptr element, const unsigned length)
    : Type(context, Kind::Array),
      m_Element(std::move(element)),
      m_Length(length)
{
}

unsigned scc::ir::ArrayType::GenerateHash() const
{
    return CombineHash(4, CombineHash(m_Element->GenerateHash(), m_Length));
}

bool scc::ir::ArrayType::Equals(const TypeFwd::Ptr &type) const
{
    if (type->GetKind() != Kind::Array)
        return false;

    if (const auto p = std::dynamic_pointer_cast < ArrayType > (type))
        return m_Element == p->m_Element && m_Length == p->m_Length;

    return false;
}

unsigned scc::ir::ArrayType::GetSize() const
{
    const auto el_size = m_Element->GetSize();
    const auto el_align = m_Element->GetAlign();

    const auto stride = AlignTo(el_size, el_align);

    return m_Length * stride;
}

unsigned scc::ir::ArrayType::GetAlign() const
{
    return m_Element->GetAlign();
}

bool scc::ir::ArrayType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::ArrayType::Print(std::ostream &stream) const
{
    return m_Element->Print(stream << '[') << " x " << m_Length << ']';
}

scc::ir::TypeFwd::Ptr scc::ir::ArrayType::GetElement() const
{
    return m_Element;
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
    Assert(index<m_Length, "element index out of bounds");
    return m_Element;
}

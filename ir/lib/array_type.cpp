#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ArrayType::ArrayType(Context &context, Type *element, const unsigned length)
    : Type(context, Kind::Array),
      m_Element(element),
      m_Length(length)
{
}

bool scc::ir::ArrayType::Compare(Type *type) const
{
    if (const auto p = dynamic_cast<ArrayType *>(type))
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

scc::ir::Type *scc::ir::ArrayType::GetElement() const
{
    return m_Element;
}

unsigned scc::ir::ArrayType::GetLength() const
{
    return m_Length;
}

size_t scc::ir::ArrayType::GetElementCount() const
{
    return m_Length;
}

scc::ir::Type *scc::ir::ArrayType::GetElement(const size_t index) const
{
    AssertIndexInBounds(index, m_Length);

    return m_Element;
}

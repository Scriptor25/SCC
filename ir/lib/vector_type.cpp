#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::VectorType::VectorType(Context &context, Type *element, const unsigned length)
    : Type(context, Kind::Vector),
      m_Element(element),
      m_Length(length)
{
}

bool scc::ir::VectorType::Compare(Type *type) const
{
    if (const auto p = dynamic_cast<VectorType *>(type))
        return m_Element == p->m_Element && m_Length == p->m_Length;

    return false;
}

unsigned scc::ir::VectorType::GetSize() const
{
    return m_Length * m_Element->GetSize();
}

unsigned scc::ir::VectorType::GetAlign() const
{
    const auto size = GetSize();
    const auto align = std::bit_ceil(size);
    return std::min(align, 16u);
}

bool scc::ir::VectorType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::VectorType::Print(std::ostream &stream) const
{
    return m_Element->Print(stream << '<') << " x " << m_Length << '>';
}

scc::ir::Type *scc::ir::VectorType::GetElement() const
{
    return m_Element;
}

unsigned scc::ir::VectorType::GetLength() const
{
    return m_Length;
}

size_t scc::ir::VectorType::GetElementCount() const
{
    return m_Length;
}

scc::ir::Type *scc::ir::VectorType::GetElement(const size_t index) const
{
    AssertIndexInBounds(index, m_Length);

    return m_Element;
}

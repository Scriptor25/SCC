#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ArrayType::ArrayType(Context &context, Type *element, const size_t element_count)
    : Type(context, Kind::Array),
      m_Element(element),
      m_ElementCount(element_count)
{
}

bool scc::ir::ArrayType::Compare(Type *type) const
{
    if (const auto *array_type = dynamic_cast<ArrayType *>(type))
        return m_Element == array_type->m_Element && m_ElementCount == array_type->m_ElementCount;

    return false;
}

size_t scc::ir::ArrayType::GetSize() const
{
    const auto el_size = m_Element->GetSize();
    const auto el_align = m_Element->GetAlign();

    const auto stride = AlignTo(el_size, el_align);

    return m_ElementCount * stride;
}

size_t scc::ir::ArrayType::GetAlign() const
{
    return m_Element->GetAlign();
}

bool scc::ir::ArrayType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::ArrayType::Print(std::ostream &stream) const
{
    return m_Element->Print(stream << '[') << " x " << std::dec << m_ElementCount << ']';
}

scc::ir::Type *scc::ir::ArrayType::GetElement() const
{
    return m_Element;
}

size_t scc::ir::ArrayType::GetElementCount() const
{
    return m_ElementCount;
}

scc::ir::Type *scc::ir::ArrayType::GetElement(const size_t index) const
{
    AssertIndexInBounds(index, m_ElementCount);

    return m_Element;
}

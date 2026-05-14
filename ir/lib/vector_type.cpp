#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>
#include <scc/platform.hpp>

scc::ir::VectorType::VectorType(Context &context, Type *element, size_t element_count)
    : Type(context, Kind::Vector),
      m_Element(element),
      m_ElementCount(element_count)
{
}

bool scc::ir::VectorType::Compare(Type *type) const
{
    if (const auto *vector_type = dynamic_cast<VectorType *>(type))
        return m_Element == vector_type->m_Element && m_ElementCount == vector_type->m_ElementCount;

    return false;
}

size_t scc::ir::VectorType::GetSize() const
{
    return m_ElementCount * m_Element->GetSize();
}

size_t scc::ir::VectorType::GetAlign() const
{
    const auto size = GetSize();
    const auto align = std::bit_ceil(size);
    return std::min<size_t>(align, m_Context.GetPlatform().ABI.DataLayout.MaxVectorAlign);
}

bool scc::ir::VectorType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::VectorType::Print(std::ostream &stream) const
{
    return m_Element->Print(stream << '<') << " x " << std::dec << m_ElementCount << '>';
}

scc::ir::Type *scc::ir::VectorType::GetElement() const
{
    return m_Element;
}

size_t scc::ir::VectorType::GetElementCount() const
{
    return m_ElementCount;
}

scc::ir::Type *scc::ir::VectorType::GetElement(const size_t index) const
{
    AssertIndexInBounds(index, m_ElementCount);

    return m_Element;
}

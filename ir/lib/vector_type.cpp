#include <scc/ir/type.hpp>

#include <scc/assert.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/platform.hpp>

scc::ir::VectorType::VectorType(Context &context, Type *element, const unsigned length)
    : Type(context, Kind::Vector),
      m_Element(element),
      m_Length(length)
{
}

bool scc::ir::VectorType::Compare(Type *type) const
{
    if (const auto *vector_type = dynamic_cast<VectorType *>(type))
        return m_Element == vector_type->m_Element && m_Length == vector_type->m_Length;

    return false;
}

size_t scc::ir::VectorType::GetSize() const
{
    return m_Length * m_Element->GetSize();
}

size_t scc::ir::VectorType::GetAlign() const
{
    const auto size = GetSize();
    const auto align = std::bit_ceil(size);
    return std::min<size_t>(align, m_Context.GetPlatform().MaxVectorAlign);
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

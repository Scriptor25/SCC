#include <scc/ir/context.hpp>
#include <scc/ir/platform.hpp>
#include <scc/ir/type.hpp>

scc::ir::StructType::StructType(Context &context, std::vector<Type *> elements)
    : Type(context, Kind::Struct),
      m_Elements(std::move(elements))
{
}

bool scc::ir::StructType::Compare(Type *type) const
{
    if (const auto p = dynamic_cast<StructType *>(type))
    {
        if (m_Elements.size() != p->m_Elements.size())
            return false;

        for (unsigned i = 0; i < m_Elements.size(); ++i)
            if (m_Elements[i] != p->m_Elements[i])
                return false;

        return true;
    }

    return false;
}

size_t scc::ir::StructType::GetSize() const
{
    size_t offset = 0;
    size_t align = 1;

    for (const auto element : m_Elements)
    {
        const auto el_size = element->GetSize();
        const auto el_align = element->GetAlign();

        offset = AlignTo(offset, el_align);
        offset += el_size;

        align = std::max(align, el_align);
    }

    return AlignTo(offset, align);
}

size_t scc::ir::StructType::GetAlign() const
{
    size_t align = 1;

    for (const auto element : m_Elements)
        align = std::max(align, element->GetAlign());

    return std::min(align, m_Context.GetPlatform().MaxAggregateAlign);
}

bool scc::ir::StructType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::StructType::Print(std::ostream &stream) const
{
    stream << '{';

    for (auto i = m_Elements.begin(); i != m_Elements.end(); ++i)
    {
        if (i != m_Elements.begin())
            stream << ',';

        (*i)->Print(stream);
    }

    return stream << '}';
}

size_t scc::ir::StructType::GetElementCount() const
{
    return m_Elements.size();
}

scc::ir::Type *scc::ir::StructType::GetElement(size_t index) const
{
    return m_Elements[index];
}

std::vector<scc::ir::Type *>::const_iterator scc::ir::StructType::begin() const
{
    return m_Elements.begin();
}

std::vector<scc::ir::Type *>::const_iterator scc::ir::StructType::end() const
{
    return m_Elements.end();
}

#include <scc/ir/type.hpp>

scc::ir::StructType::StructType(Context &context, std::vector<TypeFwd::Ptr> elements)
    : Type(context, Kind::Struct),
      m_Elements(std::move(elements))
{
}

unsigned scc::ir::StructType::GenerateHash() const
{
    unsigned hash = 0;
    for (auto &element : m_Elements)
        hash = CombineHash(hash, element->GenerateHash());
    return CombineHash(6, hash);
}

bool scc::ir::StructType::Equals(const TypeFwd::Ptr &type) const
{
    if (type->GetKind() != Kind::Struct)
        return false;

    if (const auto p = std::dynamic_pointer_cast < StructType > (type))
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

unsigned scc::ir::StructType::GetSize() const
{
    auto offset = 0u;
    auto align = 1u;

    for (auto &element : m_Elements)
    {
        const auto el_size = element->GetSize();
        const auto el_align = element->GetAlign();

        offset = AlignTo(offset, el_align);
        offset += el_size;

        align = std::max(align, el_align);
    }

    return AlignTo(offset, align);
}

unsigned scc::ir::StructType::GetAlign() const
{
    auto align = 1u;
    for (auto &element : m_Elements)
        align = std::max(align, element->GetAlign());
    return align;
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

unsigned scc::ir::StructType::GetElementCount() const
{
    return m_Elements.size();
}

scc::ir::TypeFwd::Ptr scc::ir::StructType::GetElement(const unsigned index) const
{
    return m_Elements[index];
}

std::vector<scc::ir::TypeFwd::Ptr>::const_iterator scc::ir::StructType::begin() const
{
    return m_Elements.begin();
}

std::vector<scc::ir::TypeFwd::Ptr>::const_iterator scc::ir::StructType::end() const
{
    return m_Elements.end();
}

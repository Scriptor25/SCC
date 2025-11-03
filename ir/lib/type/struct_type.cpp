#include <scc/ir/type.hpp>

scc::ir::StructType::StructType(Context &context, std::vector<TypePtr> elements)
    : Type(context, Kind_Struct),
      m_Elements(std::move(elements))
{
}

unsigned scc::ir::StructType::GenerateHash() const
{
    unsigned hash = 0;
    for (auto &element : m_Elements)
    {
        hash = CombineHash(hash, element->GenerateHash());
    }
    return CombineHash(6, hash);
}

bool scc::ir::StructType::Equals(const TypePtr &type) const
{
    if (type->GetKind() != Kind_Struct)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<StructType>(type))
    {
        if (m_Elements.size() != p->m_Elements.size())
        {
            return false;
        }

        for (unsigned i = 0; i < m_Elements.size(); ++i)
        {
            if (m_Elements.at(i) != p->m_Elements.at(i))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

unsigned scc::ir::StructType::GetElementCount() const
{
    return m_Elements.size();
}

scc::ir::TypePtr scc::ir::StructType::GetElement(const unsigned index) const
{
    return m_Elements.at(index);
}

std::vector<scc::ir::TypePtr>::const_iterator scc::ir::StructType::begin() const
{
    return m_Elements.begin();
}

std::vector<scc::ir::TypePtr>::const_iterator scc::ir::StructType::end() const
{
    return m_Elements.end();
}

#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantArray::ConstantArray(TypePtr type, std::vector<Constant::Ptr> elements)
    : Constant(std::move(type)),
      m_Elements(std::move(elements))
{
}

std::ostream &scc::ir::ConstantArray::Print(std::ostream &stream) const
{
    m_Type->Print(stream) << " const [";
    for (auto i = m_Elements.begin(); i != m_Elements.end(); ++i)
    {
        if (i != m_Elements.begin())
        {
            stream << ", ";
        }
        (*i)->Print(stream);
    }
    return stream << ']';
}

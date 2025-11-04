#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantStruct::ConstantStruct(StructType::Ptr type, std::vector<ConstantPtr> values)
    : Constant(std::move(type)),
      m_Values(std::move(values))
{
}

std::ostream &scc::ir::ConstantStruct::Print(std::ostream &stream) const
{
    m_Type->Print(stream) << " const {";
    for (auto i = m_Values.begin(); i != m_Values.end(); ++i)
    {
        if (i != m_Values.begin())
        {
            stream << ", ";
        }
        (*i)->Print(stream);
    }
    return stream << '}';
}

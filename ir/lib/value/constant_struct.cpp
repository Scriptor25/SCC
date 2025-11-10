#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantStruct::ConstantStruct(StructType::Ptr type, std::vector<ConstantFwd::Ptr> values)
    : Constant(std::move(type)),
      m_Values(std::move(values))
{
}

std::ostream &scc::ir::ConstantStruct::PrintOperand(std::ostream &stream) const
{
    m_Type->Print(stream) << " {";
    for (auto i = m_Values.begin(); i != m_Values.end(); ++i)
    {
        if (i != m_Values.begin())
        {
            stream << ", ";
        }
        (*i)->PrintOperand(stream);
    }
    return stream << '}';
}

unsigned scc::ir::ConstantStruct::GetValueCount() const
{
    return m_Values.size();
}

scc::ir::ConstantFwd::Ptr scc::ir::ConstantStruct::GetValue(const unsigned index) const
{
    return m_Values.at(index);
}

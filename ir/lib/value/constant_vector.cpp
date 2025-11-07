#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantVector::ConstantVector(VectorType::Ptr type, std::vector<ConstantPtr> values)
    : Constant(std::move(type)),
      m_Values(std::move(values))
{
}

std::ostream &scc::ir::ConstantVector::PrintOperand(std::ostream &stream) const
{
    m_Type->Print(stream) << " <";
    for (auto i = m_Values.begin(); i != m_Values.end(); ++i)
    {
        if (i != m_Values.begin())
        {
            stream << ", ";
        }
        (*i)->PrintOperand(stream);
    }
    return stream << '>';
}

unsigned scc::ir::ConstantVector::GetValueCount() const
{
    return m_Values.size();
}

scc::ir::ConstantPtr scc::ir::ConstantVector::GetValue(const unsigned index) const
{
    return m_Values.at(index);
}

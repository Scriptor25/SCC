#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ConstantVector::ConstantVector(VectorType *type, std::vector<Constant *> values)
    : Constant(type),
      m_Values(std::move(values))
{
    for (auto *value : m_Values)
        value->Use(this);
}

scc::ir::ConstantVector::~ConstantVector()
{
    DropAll();
}

void scc::ir::ConstantVector::DropAll()
{
    for (auto &value : m_Values)
        if (value)
        {
            value->Drop(this);
            value = nullptr;
        }

    m_Values.clear();
}

void scc::ir::ConstantVector::Replace(Value *value, Value *with)
{
    for (auto &val : m_Values)
        if (val == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            val = dynamic_cast<Constant *>(with);
        }
}

std::ostream &scc::ir::ConstantVector::PrintOperand(std::ostream &stream, const bool print_type) const
{
    if (print_type)
        m_Type->Print(stream) << ' ';

    stream << '<';

    for (auto i = m_Values.begin(); i != m_Values.end(); ++i)
    {
        if (i != m_Values.begin())
            stream << ", ";

        (*i)->PrintOperand(stream, false);
    }

    return stream << '>';
}

bool scc::ir::ConstantVector::Compare(Constant *value) const
{
    if (const auto *vector_value = dynamic_cast<const ConstantVector *>(value))
    {
        if (m_Type != vector_value->m_Type)
            return false;

        if (m_Values.size() != vector_value->m_Values.size())
            return false;

        for (size_t i = 0; i < m_Values.size(); ++i)
            if (m_Values[i] != vector_value->m_Values[i])
                return false;

        return true;
    }

    return false;
}

unsigned scc::ir::ConstantVector::GetValueCount() const
{
    return m_Values.size();
}

scc::ir::Constant *scc::ir::ConstantVector::GetValue(const unsigned index) const
{
    AssertIndexInBounds(index, m_Values.size());

    return m_Values[index];
}

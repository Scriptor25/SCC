#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ConstantStruct::ConstantStruct(StructType *type, std::vector<Constant *> values)
    : Constant(type),
      m_Values(std::move(values))
{
    for (const auto value : m_Values)
        value->Use(this);
}

scc::ir::ConstantStruct::~ConstantStruct()
{
    DropAll();
}

void scc::ir::ConstantStruct::DropAll()
{
    for (auto &value : m_Values)
        if (value)
        {
            value->Drop(this);
            value = nullptr;
        }
}

void scc::ir::ConstantStruct::Replace(Value *value, Value *with)
{
    for (auto &val : m_Values)
        if (val == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            val = dynamic_cast<Constant *>(with);
            return;
        }

    Constant::Replace(value, with);
}

std::ostream &scc::ir::ConstantStruct::PrintOperand(std::ostream &stream) const
{
    m_Type->Print(stream) << " {";

    for (auto i = m_Values.begin(); i != m_Values.end(); ++i)
    {
        if (i != m_Values.begin())
            stream << ", ";

        (*i)->PrintOperand(stream);
    }

    return stream << '}';
}

bool scc::ir::ConstantStruct::Compare(Constant *value) const
{
    if (const auto p = dynamic_cast<ConstantStruct *>(value))
    {
        if (m_Type != p->m_Type)
            return false;

        if (m_Values.size() != p->m_Values.size())
            return false;

        for (auto i = 0ull; i < m_Values.size(); ++i)
            if (m_Values[i] != p->m_Values[i])
                return false;

        return true;
    }

    return false;
}

unsigned scc::ir::ConstantStruct::GetValueCount() const
{
    return m_Values.size();
}

scc::ir::Constant *scc::ir::ConstantStruct::GetValue(const unsigned index) const
{
    AssertIndexInBounds(index, m_Values.size());

    return m_Values[index];
}

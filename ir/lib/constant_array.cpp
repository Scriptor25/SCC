#include <scc/ir/constant.hpp>
#include <scc/ir/context.hpp>

#include <scc/assert.hpp>

scc::ir::ConstantArray::ConstantArray(ArrayType *type, std::vector<Constant *> values)
    : Constant(type),
      m_Values(std::move(values))
{
    for (const auto value : m_Values)
        value->Use(this);
}

scc::ir::ConstantArray::~ConstantArray()
{
    DropAll();
}

void scc::ir::ConstantArray::DropAll()
{
    for (auto &value : m_Values)
        if (value)
        {
            value->Drop(this);
            value = nullptr;
        }
}

void scc::ir::ConstantArray::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::ConstantArray::PrintOperand(std::ostream &stream) const
{
    if (const auto p = dynamic_cast<ArrayType *>(m_Type); p->GetElement() == p->GetContext().GetInt8Type())
    {
        m_Type->Print(stream) << " \"";

        for (auto &value : m_Values)
            if (const auto c = dynamic_cast<ConstantInt *>(value)->GetValue(); c >= 0x20)
                stream << static_cast<char>(c);
            else
                stream << "\\x" << std::hex << (c >> 4ull & 0xFull) << (c & 0xFull) << std::dec;

        return stream << '"';
    }

    m_Type->Print(stream) << " [";

    for (auto i = m_Values.begin(); i != m_Values.end(); ++i)
    {
        if (i != m_Values.begin())
            stream << ", ";

        (*i)->PrintOperand(stream);
    }

    return stream << ']';
}

bool scc::ir::ConstantArray::Compare(Constant *value) const
{
    if (const auto p = dynamic_cast<ConstantArray *>(value))
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

unsigned scc::ir::ConstantArray::GetValueCount() const
{
    return m_Values.size();
}

scc::ir::Constant *scc::ir::ConstantArray::GetValue(const unsigned index) const
{
    AssertIndexInBounds(index, m_Values.size());

    return m_Values[index];
}

#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ConstantStruct::ConstantStruct(StructType *type, std::vector<Constant *> elements)
    : Constant(type),
      m_Elements(std::move(elements))
{
    for (auto *element : m_Elements)
        element->Use(this);
}

scc::ir::ConstantStruct::~ConstantStruct()
{
    DropAll();
}

void scc::ir::ConstantStruct::DropAll()
{
    for (auto &element : m_Elements)
        if (element)
        {
            element->Drop(this);
            element = nullptr;
        }

    m_Elements.clear();
}

void scc::ir::ConstantStruct::Replace(Value *value, Value *with)
{
    for (auto &element : m_Elements)
        if (element == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            element = dynamic_cast<Constant *>(with);
        }
}

std::ostream &scc::ir::ConstantStruct::PrintOperand(std::ostream &stream, const bool print_type) const
{
    if (print_type)
        m_Type->Print(stream) << ' ';

    stream << '{';

    for (auto i = m_Elements.begin(); i != m_Elements.end(); ++i)
    {
        if (i != m_Elements.begin())
            stream << ", ";

        (*i)->PrintOperand(stream, false);
    }

    return stream << '}';
}

bool scc::ir::ConstantStruct::Compare(Constant *value) const
{
    if (const auto *struct_value = dynamic_cast<const ConstantStruct *>(value))
    {
        if (m_Type != struct_value->m_Type)
            return false;

        if (m_Elements.size() != struct_value->m_Elements.size())
            return false;

        for (size_t i = 0; i < m_Elements.size(); ++i)
            if (m_Elements[i] != struct_value->m_Elements[i])
                return false;

        return true;
    }

    return false;
}

size_t scc::ir::ConstantStruct::GetElementCount() const
{
    return m_Elements.size();
}

scc::ir::Constant *scc::ir::ConstantStruct::GetElement(const size_t index) const
{
    AssertIndexInBounds(index, m_Elements.size());

    return m_Elements[index];
}

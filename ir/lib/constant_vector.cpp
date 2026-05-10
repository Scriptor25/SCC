#include <scc/ir/constant.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::ConstantVector::ConstantVector(VectorType *type, std::vector<Constant *> elements)
    : Constant(type),
      m_Elements(std::move(elements))
{
    for (auto *element : m_Elements)
        element->Use(this);
}

scc::ir::ConstantVector::~ConstantVector()
{
    DropAll();
}

void scc::ir::ConstantVector::DropAll()
{
    for (auto &element : m_Elements)
        if (element)
        {
            element->Drop(this);
            element = nullptr;
        }

    m_Elements.clear();
}

void scc::ir::ConstantVector::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::ConstantVector::PrintOperand(std::ostream &stream, const bool print_type) const
{
    if (print_type)
        m_Type->Print(stream) << ' ';

    stream << '<';

    for (auto i = m_Elements.begin(); i != m_Elements.end(); ++i)
    {
        if (i != m_Elements.begin())
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

        if (m_Elements.size() != vector_value->m_Elements.size())
            return false;

        for (size_t i = 0; i < m_Elements.size(); ++i)
            if (m_Elements[i] != vector_value->m_Elements[i])
                return false;

        return true;
    }

    return false;
}

size_t scc::ir::ConstantVector::GetElementCount() const
{
    return m_Elements.size();
}

scc::ir::Constant *scc::ir::ConstantVector::GetElement(const size_t index) const
{
    AssertIndexInBounds(index, m_Elements.size());

    return m_Elements[index];
}

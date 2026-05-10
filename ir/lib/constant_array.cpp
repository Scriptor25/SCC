#include <scc/ir/constant.hpp>
#include <scc/ir/context.hpp>

#include <scc/assert.hpp>

scc::ir::ConstantArray::ConstantArray(ArrayType *type, std::vector<Constant *> elements)
    : Constant(type),
      m_Elements(std::move(elements))
{
    for (auto *element : m_Elements)
        element->Use(this);
}

scc::ir::ConstantArray::~ConstantArray()
{
    DropAll();
}

void scc::ir::ConstantArray::DropAll()
{
    for (auto &element : m_Elements)
        if (element)
        {
            element->Drop(this);
            element = nullptr;
        }

    m_Elements.clear();
}

void scc::ir::ConstantArray::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::ConstantArray::PrintOperand(std::ostream &stream, const bool print_type) const
{
    if (print_type)
        m_Type->Print(stream) << ' ';

    if (const auto *array_type = dynamic_cast<ArrayType *>(m_Type);
        array_type->GetElement() == array_type->GetContext().GetInt8Type())
    {
        stream << '"';

        for (auto &element : m_Elements)
            if (const auto c = dynamic_cast<ConstantInt *>(element)->GetValue(); c >= 0x20)
                stream << static_cast<char>(c);
            else
                stream << "\\x" << std::hex << (c >> 4 & 0xF) << (c & 0xF);

        return stream << '"';
    }

    stream << '[';

    for (auto i = m_Elements.begin(); i != m_Elements.end(); ++i)
    {
        if (i != m_Elements.begin())
            stream << ", ";

        (*i)->PrintOperand(stream, false);
    }

    return stream << ']';
}

bool scc::ir::ConstantArray::Compare(Constant *value) const
{
    if (const auto *array_value = dynamic_cast<ConstantArray *>(value))
    {
        if (m_Type != array_value->m_Type)
            return false;

        if (m_Elements.size() != array_value->m_Elements.size())
            return false;

        for (size_t i = 0; i < m_Elements.size(); ++i)
            if (m_Elements[i] != array_value->m_Elements[i])
                return false;

        return true;
    }

    return false;
}

size_t scc::ir::ConstantArray::GetElementCount() const
{
    return m_Elements.size();
}

scc::ir::Constant *scc::ir::ConstantArray::GetElement(const size_t index) const
{
    AssertIndexInBounds(index, m_Elements.size());

    return m_Elements[index];
}

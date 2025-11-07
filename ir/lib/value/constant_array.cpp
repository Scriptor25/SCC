#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>
#include <scc/ir/value.hpp>

scc::ir::ConstantArray::ConstantArray(ArrayType::Ptr type, std::vector<ConstantPtr> values)
    : Constant(std::move(type)),
      m_Values(std::move(values))
{
}

std::ostream &scc::ir::ConstantArray::PrintOperand(std::ostream &stream) const
{
    if (const auto p = std::dynamic_pointer_cast<ArrayType>(m_Type);
        p->GetBase() == p->GetContext().GetI8Type())
    {
        m_Type->Print(stream) << " \"";
        for (auto &value : m_Values)
        {
            if (const auto c = std::dynamic_pointer_cast<ConstantInt>(value)->GetValue(); c >= 0x20)
            {
                stream << static_cast<char>(c);
            }
            else
            {
                stream << "\\x" << std::hex << (c >> 4ull & 0xFull) << (c & 0xFull) << std::dec;
            }
        }
        return stream << '"';
    }

    m_Type->Print(stream) << " [";
    for (auto i = m_Values.begin(); i != m_Values.end(); ++i)
    {
        if (i != m_Values.begin())
        {
            stream << ", ";
        }
        (*i)->PrintOperand(stream);
    }
    return stream << ']';
}

unsigned scc::ir::ConstantArray::GetValueCount() const
{
    return m_Values.size();
}

scc::ir::ConstantPtr scc::ir::ConstantArray::GetValue(const unsigned index) const
{
    return m_Values.at(index);
}

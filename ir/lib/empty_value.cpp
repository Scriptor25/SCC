#include <scc/ir/empty_value.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::EmptyValue::EmptyValue(Type *type, std::string name)
    : Value(type),
      m_Name(std::move(name))
{
}

std::ostream &scc::ir::EmptyValue::Print(std::ostream &stream) const
{
    if (m_Name.empty())
        return stream << "<empty>";

    return m_Type->Print(stream << '%' << m_Name << " = ") << " <empty>";
}

std::ostream &scc::ir::EmptyValue::PrintOperand(std::ostream &stream, const bool print_type) const
{
    Assert(!m_Name.empty(), "cannot use nameless value");

    if (print_type)
        m_Type->Print(stream) << ' ';

    return stream << '%' << m_Name;
}

std::ostream &scc::ir::EmptyValue::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("cannot print empty value assembly");
}

std::ostream &scc::ir::EmptyValue::PrintOperandAssembly(
    std::ostream &stream,
    LoweringContext &context,
    bool address) const
{
    Error("cannot print empty value operand assembly");
}

void scc::ir::EmptyValue::SetName(std::string name)
{
    m_Name = std::move(name);
}

const std::string &scc::ir::EmptyValue::GetName() const
{
    return m_Name;
}

#include <scc/ir/register.hpp>
#include <scc/ir/value.hpp>

#include <scc/assert.hpp>

scc::ir::IdentifiedValue::IdentifiedValue(TypeFwd::Ptr type, RegisterFwd::Ptr register_)
    : Value(std::move(type)),
      m_Register(std::move(register_))
{
}

scc::ir::RegisterFwd::Ptr scc::ir::IdentifiedValue::GetRegister() const
{
    return m_Register;
}

void scc::ir::IdentifiedValue::SetRegister(RegisterFwd::Ptr register_)
{
    Assert(!!register_, "register must not be null");

    m_Register = std::move(register_);
}

void scc::ir::IdentifiedValue::ClearRegister()
{
    m_Register = nullptr;
}

std::ostream &scc::ir::IdentifiedValue::Print(std::ostream &stream) const
{
    if (m_Register)
        return m_Register->Print(stream) << " = <error>";
    return stream << "<error>";
}

std::ostream &scc::ir::IdentifiedValue::PrintOperand(std::ostream &stream) const
{
    if (m_Register)
        return m_Register->Print(stream);
    return stream << "<error>";
}

#include <scc/ir/register.hpp>
#include <scc/ir/value.hpp>

scc::ir::Argument::Argument(TypeFwd::Ptr type, RegisterFwd::Ptr register_)
    : IdentifiedValue(std::move(type), std::move(register_))
{
}

std::ostream &scc::ir::Argument::Print(std::ostream &stream) const
{
    return m_Register->Print(m_Type->Print(stream) << ' ');
}

void scc::ir::Argument::SetName(std::string name) const
{
    m_Register->SetName(std::move(name));
}

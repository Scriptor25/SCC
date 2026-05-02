#include <scc/ir/context.hpp>
#include <scc/ir/global.hpp>

scc::ir::Global::Global(Type *type, std::string name)
    : Constant(type->GetContext().GetPointerType(type)),
      m_Name(std::move(name))
{
}

std::ostream &scc::ir::Global::PrintOperand(std::ostream &stream) const
{
    return stream << '@' << m_Name;
}

bool scc::ir::Global::Compare(Constant *value) const
{
    if (const auto p = dynamic_cast<Global *>(value))
        return m_Name == p->m_Name;

    return false;
}

const std::string &scc::ir::Global::GetName() const
{
    return m_Name;
}

void scc::ir::Global::SetName(std::string name)
{
    m_Name = std::move(name);
}

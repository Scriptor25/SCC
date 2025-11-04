#include <scc/ir/type.hpp>

scc::ir::PointerType::PointerType(Context &context, TypePtr base)
    : Type(context, Kind_Pointer),
      m_Base(std::move(base))
{
}

unsigned scc::ir::PointerType::GenerateHash() const
{
    return CombineHash(3, m_Base->GenerateHash());
}

bool scc::ir::PointerType::Equals(const TypePtr &type) const
{
    if (type->GetKind() != Kind_Pointer)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<PointerType>(type))
    {
        return m_Base == p->m_Base;
    }

    return false;
}

std::ostream &scc::ir::PointerType::Print(std::ostream &stream) const
{
    return m_Base->Print(stream << '[') << ']';
}

scc::ir::TypePtr scc::ir::PointerType::GetBase() const
{
    return m_Base;
}

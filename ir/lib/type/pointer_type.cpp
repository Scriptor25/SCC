#include <scc/ir/type.hpp>

scc::ir::PointerType::PointerType(Context &context, TypeFwd::Ptr base)
    : Type(context, Kind_Pointer),
      m_Base(std::move(base))
{
}

unsigned scc::ir::PointerType::GenerateHash() const
{
    return CombineHash(3, m_Base->GenerateHash());
}

bool scc::ir::PointerType::Equals(const TypeFwd::Ptr &type) const
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

scc::ir::TypeFwd::Ptr scc::ir::PointerType::GetBase() const
{
    return m_Base;
}

unsigned scc::ir::PointerType::GetElementCount() const
{
    return ~0u;
}

scc::ir::Shared<scc::ir::Type>::Ptr scc::ir::PointerType::GetElement(unsigned index) const
{
    return m_Base;
}

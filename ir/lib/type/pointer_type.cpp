#include <scc/ir/context.hpp>
#include <scc/ir/platform.hpp>
#include <scc/ir/type.hpp>

scc::ir::PointerType::PointerType(Context &context, TypeFwd::Ptr element)
    : Type(context, Kind::Pointer),
      m_Element(std::move(element))
{
}

unsigned scc::ir::PointerType::GenerateHash() const
{
    return CombineHash(3, m_Element->GenerateHash());
}

bool scc::ir::PointerType::Equals(const TypeFwd::Ptr &type) const
{
    if (type->GetKind() != Kind::Pointer)
        return false;

    if (const auto p = std::dynamic_pointer_cast < PointerType > (type))
        return m_Element == p->m_Element;

    return false;
}

unsigned scc::ir::PointerType::GetSize() const
{
    return m_Context.GetPlatform().PointerSize;
}

unsigned scc::ir::PointerType::GetAlign() const
{
    return m_Context.GetPlatform().PointerSize;
}

bool scc::ir::PointerType::IsElement() const
{
    return true;
}

std::ostream &scc::ir::PointerType::Print(std::ostream &stream) const
{
    return m_Element->Print(stream << '[') << ']';
}

scc::ir::TypeFwd::Ptr scc::ir::PointerType::GetElement() const
{
    return m_Element;
}

unsigned scc::ir::PointerType::GetElementCount() const
{
    return ~0u;
}

scc::ir::Shared<scc::ir::Type>::Ptr scc::ir::PointerType::GetElement(unsigned index) const
{
    return m_Element;
}

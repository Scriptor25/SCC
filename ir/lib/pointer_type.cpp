#include <scc/ir/context.hpp>
#include <scc/ir/platform.hpp>
#include <scc/ir/type.hpp>

scc::ir::PointerType::PointerType(Context &context, Type *element)
    : Type(context, Kind::Pointer),
      m_Element(element)
{
}

bool scc::ir::PointerType::Compare(Type *type) const
{
    if (const auto p = dynamic_cast<PointerType *>(type))
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

scc::ir::Type *scc::ir::PointerType::GetElement() const
{
    return m_Element;
}

size_t scc::ir::PointerType::GetElementCount() const
{
    return ~0u;
}

scc::ir::Type *scc::ir::PointerType::GetElement(size_t index) const
{
    return m_Element;
}

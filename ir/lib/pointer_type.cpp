#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

#include <scc/platform.hpp>

scc::ir::PointerType::PointerType(Context &context, Type *element)
    : Type(context, Kind::Pointer),
      m_Element(element)
{
}

bool scc::ir::PointerType::Compare(Type *type) const
{
    if (const auto *pointer_type = dynamic_cast<PointerType *>(type))
        return m_Element == pointer_type->m_Element;

    return false;
}

size_t scc::ir::PointerType::GetSize() const
{
    return m_Context.GetPlatform().ABI.DataLayout.PointerSize;
}

size_t scc::ir::PointerType::GetAlign() const
{
    return m_Context.GetPlatform().ABI.DataLayout.PointerAlign;
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

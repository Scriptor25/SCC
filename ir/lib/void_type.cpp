#include <scc/ir/type.hpp>

scc::ir::VoidType::VoidType(Context &context)
    : Type(context, Kind::Void)
{
}

bool scc::ir::VoidType::Compare(Type *type) const
{
    return type->GetKind() == Kind::Void;
}

unsigned scc::ir::VoidType::GetSize() const
{
    return 0u;
}

unsigned scc::ir::VoidType::GetAlign() const
{
    return 1u;
}

bool scc::ir::VoidType::IsElement() const
{
    return false;
}

std::ostream &scc::ir::VoidType::Print(std::ostream &stream) const
{
    return stream << "void";
}

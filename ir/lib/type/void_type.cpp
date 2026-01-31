#include <scc/ir/type.hpp>

scc::ir::VoidType::VoidType(Context &context)
    : Type(context, Kind::Void)
{
}

unsigned scc::ir::VoidType::GenerateHash() const
{
    return 0u;
}

bool scc::ir::VoidType::Equals(const TypeFwd::Ptr &type) const
{
    return type->GetKind() == Kind::Void;
}

std::ostream &scc::ir::VoidType::Print(std::ostream &stream) const
{
    return stream << "void";
}

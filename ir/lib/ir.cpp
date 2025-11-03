#include <scc/ir/ir.hpp>
#include <scc/ir/type.hpp>

unsigned scc::ir::TypeHash::operator()(const TypePtr &type) const noexcept
{
    return type->GenerateHash();
}

bool scc::ir::operator==(const TypePtr &a, const TypePtr &b)
{
    return a.get() == b.get() || (a && b && a->GetKind() == b->GetKind() && a->Equals(b));
}

unsigned scc::ir::CombineHash(const unsigned a, const unsigned b)
{
    return a ^ b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2);
}

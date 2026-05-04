#include <scc/ir/type.hpp>

#include <scc/error.hpp>

scc::ir::Type::Type(Context &context, const Kind kind)
    : m_Context(context),
      m_Kind(kind)
{
}

scc::ir::Context &scc::ir::Type::GetContext() const
{
    return m_Context;
}

scc::ir::Kind scc::ir::Type::GetKind() const
{
    return m_Kind;
}

size_t scc::ir::Type::GetElementCount() const
{
    return 0ull;
}

scc::ir::Type *scc::ir::Type::GetElement(const size_t index) const
{
    ErrorIndexOutOfBounds(index, 0ull);
}

std::ostream &scc::ir::operator<<(std::ostream &stream, const Type *type)
{
    return type->Print(stream);
}

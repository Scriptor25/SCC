#include <scc/ir/type.hpp>

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

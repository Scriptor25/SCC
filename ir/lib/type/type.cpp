#include <scc/error.hpp>
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

unsigned scc::ir::Type::GetElementCount() const
{
    return 0u;
}

scc::ir::TypeFwd::Ptr scc::ir::Type::GetElement(unsigned index) const
{
    Error("element index out of bounds");
}

#include <scc/ir/value.hpp>

scc::ir::Value::Value(TypeFwd::Ptr type)
    : m_Type(std::move(type))
{
}

scc::ir::TypeFwd::Ptr scc::ir::Value::GetType() const
{
    return m_Type;
}

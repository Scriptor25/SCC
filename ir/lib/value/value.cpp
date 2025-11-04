#include <scc/ir/value.hpp>

scc::ir::Value::Value(TypePtr type)
    : m_Type(std::move(type))
{
}

scc::ir::TypePtr scc::ir::Value::GetType() const
{
    return m_Type;
}

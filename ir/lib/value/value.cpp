#include <scc/ir/value.hpp>

scc::ir::Value::Value(TypePtr type)
    : m_Type(std::move(type))
{
}

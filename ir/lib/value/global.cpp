#include <scc/ir/value.hpp>

scc::ir::Global::Global(TypePtr type, std::string name)
    : Value(std::move(type)),
      m_Name(std::move(name))
{
}

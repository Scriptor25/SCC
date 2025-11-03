#include <scc/ir/value.hpp>

scc::ir::ConstantVector::ConstantVector(TypePtr type, std::vector<Constant> elements)
    : Constant(std::move(type)),
      m_Elements(std::move(elements))
{
}

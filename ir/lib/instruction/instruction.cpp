#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::Instruction::Instruction(
    TypePtr type,
    std::string name)
    : Value(std::move(type)),
      m_Name(std::move(name))
{
}

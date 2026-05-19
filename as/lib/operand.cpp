#include <scc/as/operand.hpp>

#include <scc/error.hpp>

scc::as::Operand::Operand(const Platform &platform)
    : m_Platform(platform)
{
}

scc::as::Immediate scc::as::Operand::GetImmediate() const
{
    Error("TODO");
}

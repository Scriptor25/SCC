#include <scc/as/operand.hpp>

#include <scc/assert.hpp>

scc::as::Operand::Operand(const Platform &platform)
    : m_Platform(platform)
{
}

const scc::Platform &scc::as::Operand::GetPlatform() const
{
    return m_Platform;
}

scc::as::Immediate scc::as::Operand::GetImmediate() const
{
    Error("operand is not immediate");
}

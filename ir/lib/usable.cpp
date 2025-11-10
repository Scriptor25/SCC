#include <scc/ir/usable.hpp>

void scc::ir::Usable::Use()
{
    m_UseCount++;
}

void scc::ir::Usable::Drop()
{
    m_UseCount--;
}

unsigned scc::ir::Usable::GetUseCount() const
{
    return m_UseCount;
}

bool scc::ir::Usable::IsUsed() const
{
    return !!m_UseCount;
}

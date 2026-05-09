#include <scc/as/section.hpp>
#include <scc/as/symbol.hpp>

scc::as::Symbol::Symbol(Fragment &fragment)
    : m_Fragment(&fragment)
{
}

scc::as::Symbol::Symbol(Section &section, const size_t index)
    : m_Fragment(&section[index])
{
}

scc::as::Fragment &scc::as::Symbol::operator*() const
{
    return *m_Fragment;
}

scc::as::Fragment *scc::as::Symbol::operator->() const
{
    return m_Fragment;
}

scc::as::Symbol::operator bool() const
{
    return m_Fragment != nullptr;
}

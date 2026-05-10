#include <scc/as/section.hpp>
#include <scc/as/symbol.hpp>

scc::as::Symbol::Symbol(std::string name)
    : m_Name(std::move(name)),
      m_Fragment()
{
}

scc::as::Symbol::Symbol(Fragment &fragment, std::string name)
    : m_Name(std::move(name)),
      m_Fragment(&fragment)
{
}

scc::as::Symbol::Symbol(Section &section, const size_t index, std::string name)
    : m_Name(std::move(name)),
      m_Fragment(&section[index])
{
}

void scc::as::Symbol::SetName(std::string name)
{
    m_Name = std::move(name);
}

void scc::as::Symbol::SetFragment(Fragment &fragment)
{
    m_Fragment = &fragment;
}

void scc::as::Symbol::SetFragment(Section &section, const size_t index)
{
    m_Fragment = &section[index];
}

const std::string &scc::as::Symbol::GetName() const
{
    return m_Name;
}

scc::as::Fragment *scc::as::Symbol::GetFragment() const
{
    return m_Fragment;
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

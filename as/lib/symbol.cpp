#include <scc/as/section.hpp>
#include <scc/as/symbol.hpp>

#include <scc/assert.hpp>

scc::as::Symbol::Symbol(std::string name)
    : m_Name(std::move(name)),
      m_Resolved(),
      m_Section(),
      m_Index()
{
}

scc::as::Symbol::Symbol(Section &section, const size_t index, std::string name)
    : m_Name(std::move(name)),
      m_Resolved(true),
      m_Section(&section),
      m_Index(index)
{
}

void scc::as::Symbol::SetName(std::string name)
{
    m_Name = std::move(name);
}

void scc::as::Symbol::SetLocation(Section &section, const size_t index)
{
    m_Resolved = true;
    m_Section = &section;
    m_Index = index;
}

void scc::as::Symbol::SetAddress(const size_t address)
{
    m_Resolved = true;
    m_Section = {};
    m_Index = address;
}

const std::string &scc::as::Symbol::GetName() const
{
    return m_Name;
}

bool scc::as::Symbol::IsResolved() const
{
    return m_Resolved;
}

scc::as::Section *scc::as::Symbol::GetSection() const
{
    return m_Section;
}

size_t scc::as::Symbol::GetIndex() const
{
    return m_Index;
}

scc::as::Fragment *scc::as::Symbol::GetFragment() const
{
    if (!m_Section)
        return nullptr;

    return &(*m_Section)[m_Index];
}

scc::as::Immediate scc::as::Symbol::GetAddress() const
{
    if (!m_Section)
        return m_Index;

    Error("symbol is not a constant address");
}

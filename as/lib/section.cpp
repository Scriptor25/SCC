#include <scc/as/section.hpp>

scc::as::Section::Section(std::string name)
    : m_Name(std::move(name))
{
}

void scc::as::Section::SetName(std::string name)
{
    m_Name = std::move(name);
}

void scc::as::Section::Insert(std::unique_ptr<Fragment> fragment)
{
    m_Fragments.push_back(std::move(fragment));
}

const std::string &scc::as::Section::GetName() const
{
    return m_Name;
}

size_t scc::as::Section::GetFragmentCount() const
{
    return m_Fragments.size();
}

scc::as::Fragment &scc::as::Section::operator[](const size_t index)
{
    return *m_Fragments[index];
}

const scc::as::Fragment &scc::as::Section::operator[](const size_t index) const
{
    return *m_Fragments[index];
}

scc::as::Section::iterator<false> scc::as::Section::begin()
{
    return { size_t(), m_Fragments };
}

scc::as::Section::iterator<false> scc::as::Section::end()
{
    return { ~size_t(), m_Fragments };
}

scc::as::Section::iterator<true> scc::as::Section::begin() const
{
    return { size_t(), m_Fragments };
}

scc::as::Section::iterator<true> scc::as::Section::end() const
{
    return { ~size_t(), m_Fragments };
}

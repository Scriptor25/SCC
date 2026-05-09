#include <scc/as/section.hpp>

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

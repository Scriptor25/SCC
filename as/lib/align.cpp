#include <scc/as/align.hpp>

#include <format>

scc::as::Align::Align(const uint8_t alignment)
    : m_Alignment(alignment)
{
}

uint8_t scc::as::Align::GetAlignment() const
{
    return m_Alignment;
}

std::ostream &scc::as::Align::Print(std::ostream &stream) const
{
    return stream << std::format(".align {}", m_Alignment);
}

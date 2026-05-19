#include <scc/as/fill.hpp>

#include <format>

scc::as::Fill::Fill(const uint64_t count, const uint8_t value)
    : m_Count(count),
      m_Value(value)
{
}

uint64_t scc::as::Fill::GetCount() const
{
    return m_Count;
}

uint8_t scc::as::Fill::GetValue() const
{
    return m_Value;
}

std::ostream &scc::as::Fill::Print(std::ostream &stream) const
{
    return stream << std::format(".fill 0x{:x}, 0x{:02x}", m_Count, m_Value);
}

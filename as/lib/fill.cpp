#include <scc/as/fill.hpp>

#include <iomanip>

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
    return stream
           << ".fill 0x"
           << std::hex
           << m_Count
           << ", 0x"
           << std::hex
           << std::setfill('0')
           << std::setw(2)
           << m_Value;
}

void scc::as::Fill::Encode(std::vector<uint8_t> &buffer, SymbolTable &, FixupTable &) const
{
    buffer.resize(buffer.size() + m_Count, m_Value);
}

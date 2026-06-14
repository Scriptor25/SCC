#include <scc/as/fill.hpp>

#include <iomanip>

scc::as::Fill::Fill(const uint64_t count, const uint8_t size, const uint64_t value)
    : m_Count(count),
      m_Size(size),
      m_Value(value)
{
}

uint64_t scc::as::Fill::GetCount() const
{
    return m_Count;
}

uint8_t scc::as::Fill::GetSize() const
{
    return m_Size;
}

uint64_t scc::as::Fill::GetValue() const
{
    return m_Value;
}

std::ostream &scc::as::Fill::Print(std::ostream &stream) const
{
    stream
            << ".fill "
            << std::dec
            << m_Count;

    if (m_Size != 1)
        stream << ", "
                << std::dec
                << m_Size;

    if (m_Value)
        stream << ", "
                << std::dec
                << m_Value;

    return stream;
}

void scc::as::Fill::Encode(std::vector<uint8_t> &buffer, SymbolTable &, FixupTable &) const
{
    buffer.resize(buffer.size() + m_Count, m_Value);
}

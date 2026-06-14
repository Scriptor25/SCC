#include <scc/as/align.hpp>

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
    return stream << ".align " << std::dec << static_cast<unsigned>(m_Alignment);
}

void scc::as::Align::Encode(std::vector<uint8_t> &buffer, SymbolTable &, FixupTable &) const
{
    const auto rem = buffer.size() % m_Alignment;

    if (!rem)
        return;

    buffer.resize(buffer.size() + m_Alignment - rem);
}

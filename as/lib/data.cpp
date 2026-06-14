#include <scc/as/data.hpp>

#include <iomanip>

scc::as::Data::Data(std::vector<uint8_t> data)
    : m_Data(std::move(data))
{
}

scc::as::Data::Data(std::span<const uint8_t> data)
    : m_Data(data.begin(), data.end())
{
}

std::span<const uint8_t> scc::as::Data::GetData() const
{
    return m_Data;
}

size_t scc::as::Data::GetDataSize() const
{
    return m_Data.size();
}

std::ostream &scc::as::Data::Print(std::ostream &stream) const
{
    stream << ".data ";
    for (auto it = m_Data.begin(); it != m_Data.end(); ++it)
    {
        if (it != m_Data.begin())
            stream << ", ";
        stream << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(*it);
    }
    return stream;
}

void scc::as::Data::Encode(std::vector<uint8_t> &buffer, SymbolTable &, FixupTable &) const
{
    buffer.insert(buffer.end(), m_Data.begin(), m_Data.end());
}

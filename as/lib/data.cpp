#include <scc/as/data.hpp>

#include <format>

scc::as::Data::Data(std::vector<uint8_t> data)
    : m_Data(std::move(data))
{
}

const std::vector<uint8_t> &scc::as::Data::GetData() const
{
    return m_Data;
}

std::ostream &scc::as::Data::Print(std::ostream &stream) const
{
    stream << ".byte ";
    for (auto it = m_Data.begin(); it != m_Data.end(); ++it)
    {
        if (it != m_Data.begin())
            stream << ", ";
        stream << std::format("0x{:02X}", *it);
    }
    return stream;
}

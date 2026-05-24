#include <scc/binary.hpp>

#include <ostream>
#include <vector>

std::string to_hex_string(uint64_t value, const uint8_t digits)
{
    std::vector<char> buffer(digits);

    auto it = buffer.rbegin();
    while (value && it != buffer.rend())
    {
        const auto rem = value % 0x10;
        const auto div = value / 0x10;

        *it++ = static_cast<char>(rem + (rem < 0xA ? '0' : 'A' - 0xA));

        value = div;
    }

    while (it != buffer.rend())
        *it++ = '0';

    return { buffer.begin(), buffer.end() };
}

void scc::PrintBinary(std::ostream &stream, const std::span<const uint8_t> buffer)
{
    static constexpr size_t block = 8;
    static constexpr size_t stride = 4 * block;

    for (size_t i = 0; i < buffer.size(); i += stride)
    {
        stream << to_hex_string(i, 8) << ' ' << '|' << ' ';

        for (size_t j = 0; j < stride; j += block)
        {
            if (j)
                stream << ' ' << ' ';

            for (size_t k = 0; k < block; ++k)
            {
                const auto offset = i + j + k;
                const auto value = offset < buffer.size() ? buffer[offset] : uint8_t{};

                if (k)
                    stream << ' ';

                stream << to_hex_string(value, 2);
            }
        }

        stream << ' ' << '|' << ' ';

        for (size_t j = 0; j < stride; ++j)
        {
            const auto offset = i + j;
            const auto value = offset < buffer.size() ? buffer[offset] : uint8_t{};

            stream << (value < 0x20 ? '.' : static_cast<char>(value));
        }

        stream << '\n';
    }
}

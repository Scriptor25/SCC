#pragma once

#include <array>
#include <cstdint>

namespace scc
{
    template<size_t N, typename T>
    std::array<uint8_t, N> ToBytes(T value, bool le)
    {
        std::array<uint8_t, N> array;

        for (size_t i = 0; i < N; ++i)
            array[i] = static_cast<uint8_t>(value >> ((le ? N - i - 1 : i) * 8) & 0xFF);

        return array;
    }
}

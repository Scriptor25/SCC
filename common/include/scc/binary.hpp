#pragma once

#include <cstdint>
#include <iosfwd>
#include <span>

namespace scc
{
    void PrintBinary(std::ostream &stream, std::span<const uint8_t>buffer);
}

#include <scc/assert.hpp>
#include <scc/error.hpp>

void scc::ErrorIndexOutOfBounds(size_t index, size_t count)
{
    Error("index {} out of bounds [0;{})", index, count);
}

void scc::AssertIndexInBounds(const size_t index, const size_t count)
{
    if (index < count)
        return;

    ErrorIndexOutOfBounds(index, count);
}

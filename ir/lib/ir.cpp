#include <scc/ir/ir.hpp>

unsigned scc::ir::AlignTo(const unsigned value, const unsigned align)
{
    return (value + align - 1u) & ~(align - 1u);
}

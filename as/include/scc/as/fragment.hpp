#pragma once

#include <scc/as/as.hpp>

#include <iosfwd>
#include <vector>

namespace scc::as
{
    class Fragment
    {
    public:
        virtual ~Fragment() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual void Encode(std::vector<uint8_t> &buffer, SymbolTable &symbol_table, FixupTable &fixup_table) const = 0;
    };
}

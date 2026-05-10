#pragma once

#include <scc/as/as.hpp>

#include <iosfwd>

namespace scc::as
{
    class Fragment
    {
    public:
        virtual ~Fragment() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
    };
}

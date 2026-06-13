#pragma once

#include <scc/ir/ir.hpp>

#include <scc/common.hpp>

#include <unordered_map>

namespace scc::ir
{
    struct LoweringContext
    {
        const Platform &TargetPlatform;

        unsigned StackSize;
        std::unordered_map<const Value *, unsigned> StackOffset;

        std::unordered_map<const Value *, Register> RegisterAssignment;
    };
}

#pragma once

#include <scc/target.hpp>

namespace scc
{
    struct Platform
    {
        platform::IntermediateRepresentation IR;
        platform::InstructionSetArchitecture ISA;
        platform::ApplicationBinaryInterface ABI;
        platform::TargetFeatures Features;
    };
}

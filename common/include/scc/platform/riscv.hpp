#pragma once

#include <scc/target.hpp>

#include <toolkit/result.hxx>

namespace scc::platform::riscv
{
    toolkit::result<IntermediateRepresentation> CreateIntermediateRepresentation(const Options &options);
    toolkit::result<InstructionSetArchitecture> CreateInstructionSetArchitecture(const Options &options);
}

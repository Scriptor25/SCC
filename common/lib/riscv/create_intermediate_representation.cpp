#include <scc/platform/riscv.hpp>

toolkit::result<scc::platform::IntermediateRepresentation> scc::platform::riscv::CreateIntermediateRepresentation(
    const Options &options)
{
    switch (options.BitWidth)
    {
    case 32:
        return IntermediateRepresentation
        {
            .LegalIntWidth = { 1, 8, 16, 32 },
            .LegalFloatWidth = { 32 },
        };

    case 64:
        return IntermediateRepresentation
        {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64 },
        };

    default:
        return toolkit::make_error("invalid bitwidth {}", options.BitWidth);
    }
}

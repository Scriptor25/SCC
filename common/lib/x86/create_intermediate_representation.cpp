#include <scc/platform/x86.hpp>

toolkit::result<scc::platform::IntermediateRepresentation> scc::platform::x86::CreateIntermediateRepresentation(
    const Options &options)
{
    switch (options.BitWidth)
    {
    case 32:
    case 64:
        return IntermediateRepresentation
        {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64, 80 },
        };

    default:
        return toolkit::make_error("invalid bitwidth {}", options.BitWidth);
    }
}

#include <scc/ir/context.hpp>
#include <scc/ir/module.hpp>
#include <scc/ir/parser.hpp>

#include <scc/platform.hpp>

#include <fstream>
#include <iostream>

int main(int argc, const char **argv)
{
    if (argc != 2)
        return 1;

    std::ifstream in(argv[1]);
    if (!in)
        return 1;

    const scc::Platform platform
    {
        .IR = {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64 },
        },
        .ISA = {
            .Architecture = scc::platform::TargetArchitecture::X86,
            .Endianness = scc::platform::TargetEndianness::LittleEndian,
        },
        .ABI = {
            .DataLayout = {
                .PointerSize = 8,
                .PointerAlign = 8,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
            },
        },
        .Features = {},
    };

    scc::ir::Context context(platform);
    scc::ir::Parser parser(in);

    auto module = parser.ParseModule(context);

    module.Print(std::cout);

    return 0;
}

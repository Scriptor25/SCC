#include <scc/ir/context.hpp>
#include <scc/ir/module.hpp>
#include <scc/ir/parser.hpp>
#include <scc/ir/platform.hpp>
#include <scc/ir/type.hpp>

#include <fstream>
#include <iostream>

int main(int argc, const char **argv)
{
    if (argc != 2)
        return 1;

    std::ifstream in(argv[1]);
    if (!in)
        return 1;

    const scc::ir::Platform platform
    {
        .Endianness = scc::ir::Endian::Little,
        .PointerSize = 8,
        .PointerAlign = 8,
        .StackAlign = 16,
        .MaxIntAlign = 8,
        .MaxFloatAlign = 8,
        .MaxVectorAlign = 16,
        .MaxAggregateAlign = 16,
        .LegalIntWidth = { 8, 16, 32, 64 },
        .LegalFloatWidth = { 32, 64 },
    };

    scc::ir::Context context(platform);
    scc::ir::Parser parser(in);

    auto module = parser.ParseModule(context);

    module.Print(std::cout);

    return 0;
}

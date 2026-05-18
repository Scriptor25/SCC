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

    auto triple = scc::platform::ParseTriple("x86_64-none-elf");
    auto platform = triple.GetPlatform({});

    scc::ir::Context context(platform);
    scc::ir::Parser parser(in);

    auto module = parser.ParseModule(context);

    module.Print(std::cout);

    return 0;
}

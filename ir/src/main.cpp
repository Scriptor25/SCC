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

    scc::platform::TargetTriple triple;
    if (auto res = scc::platform::ParseTriple("x86_64-linux-elf") >> triple; !res)
    {
        std::cerr << res.error() << std::endl;
        return 1;
    }

    scc::Platform platform;
    if (auto res = triple.GetPlatform() >> platform; !res)
    {
        std::cerr << res.error() << std::endl;
        return 1;
    }

    scc::ir::Context context(platform);

    scc::ir::Module module;
    scc::ir::Parser parser(in, context, module);

    parser.Parse();

    module.Print(std::cout);

    module.PrintAssembly(std::cout, platform);

    return 0;
}

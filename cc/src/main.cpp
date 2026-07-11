#include <scc/cc/context.hpp>
#include <scc/cc/module.hpp>
#include <scc/cc/parser.hpp>

#include <scc/platform.hpp>
#include <scc/target.hpp>

#include <scc/ir/context.hpp>
#include <scc/ir/module.hpp>

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

    scc::ir::Context ir_context(platform);
    scc::cc::Context context(ir_context);

    scc::ir::Module ir_module;
    scc::cc::Module module(ir_module);

    scc::cc::Parser parser(in, context, module);

    parser.Parse();

    ir_module.Print(std::cout);

    ir_module.PrintAssembly(std::cout, platform);

    return 0;
}

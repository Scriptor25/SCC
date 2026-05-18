#include <scc/as/module.hpp>
#include <scc/as/parser.hpp>

#include <scc/platform.hpp>

#include <fstream>
#include <iostream>

int main(const int argc, const char **argv)
{
    if (argc != 2)
        return 1;

    std::ifstream in(argv[1]);
    if (!in)
        return 1;

    auto triple = scc::platform::ParseTriple("x86_64-none-elf");
    auto platform = triple.GetPlatform({});

    scc::as::Module module;
    scc::as::Parser parser(in, platform, module);

    parser.Parse();

    module.Print(std::cout);

    return 0;
}

#include <scc/as/module.hpp>
#include <scc/as/parser.hpp>

#include <scc/binary.hpp>
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

    scc::as::Module module(platform);
    scc::as::Parser parser(in, module);

    if (auto res = parser.Parse(); !res)
    {
        std::cerr << res.error() << std::endl;
        return 1;
    }

    module.Print(std::cerr);

    std::vector<uint8_t> buffer;
    module.Encode(buffer);

    scc::PrintBinary(std::cout, buffer);

    return 0;
}

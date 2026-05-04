#include <fstream>
#include <scc/as/parser.hpp>

int main(const int argc, const char **argv)
{
    if (argc != 2)
        return 1;

    std::ifstream in(argv[1]);
    if (!in)
        return 1;

    scc::as::Parser parser(in);

    parser.Parse();

    return 0;
}

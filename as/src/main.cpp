#include <fstream>
#include <scc/as/parser.hpp>

int main(int argc, char **argv)
{
    std::ifstream stream(argv[1]);
    if (!stream.is_open())
        return 1;

    scc::as::Parser parser(stream);
    parser.Parse();

    stream.close();
    return 0;
}

#pragma once

#include <iosfwd>

namespace scc::cc
{
    class Parser
    {
    public:
        explicit Parser(std::istream & stream);

    private:
        std::istream &m_Stream;
    };
}

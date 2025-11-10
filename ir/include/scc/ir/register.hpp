#pragma once

#include <string>

namespace scc::ir
{
    class Register
    {
    public:
        explicit Register(std::string name = {});

        std::string GetName() const;
        void SetName(std::string name);

        std::ostream &Print(std::ostream &stream) const;

    private:
        std::string m_Name;
    };
}

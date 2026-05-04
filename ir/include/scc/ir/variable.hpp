#pragma once

#include <scc/ir/global.hpp>

#include <iosfwd>
#include <string>

namespace scc::ir
{
    class Variable final : public Global
    {
    public:
        explicit Variable(Type *type, std::string name, Constant *initializer);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Constant *m_Initializer;
    };
}

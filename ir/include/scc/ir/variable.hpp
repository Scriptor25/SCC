#pragma once

#include <scc/ir/global.hpp>

#include <iosfwd>
#include <string>

namespace scc::ir
{
    class Variable : public Global
    {
    public:
        explicit Variable(Type *type, std::string name, Constant *initializer);

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

    private:
        Constant *m_Initializer;
    };
}

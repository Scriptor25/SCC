#pragma once

#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

#include <iosfwd>
#include <string>

namespace scc::ir
{
    class Argument : public Value
    {
    public:
        explicit Argument(Type *type);
        explicit Argument(Type *type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;
        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        void SetName(std::string name);
        [[nodiscard]] const std::string &GetName() const;

    private:
        std::string m_Name;
    };
}

#pragma once

#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class EmptyValue final : public Value
    {
    public:
        explicit EmptyValue(Type *type, std::string name = {});

        std::ostream &Print(std::ostream &stream) const override;
        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        void SetName(std::string name);
        const std::string &GetName() const;

    private:
        std::string m_Name;
    };
}

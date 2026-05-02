#pragma once

#include <scc/ir/value.hpp>

#include <string>

namespace scc::ir
{
    class Argument final : public Value
    {
    public:
        explicit Argument(Type *type);
        explicit Argument(Type *type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;
        std::ostream &PrintOperand(std::ostream &stream) const override;

        void SetName(std::string name);
        [[nodiscard]] const std::string &GetName() const;

    private:
        std::string m_Name;
    };
}

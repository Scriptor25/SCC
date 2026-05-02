#pragma once

#include <scc/ir/constant.hpp>

namespace scc::ir
{
    class Global : public Constant
    {
    public:
        explicit Global(Type *type, std::string name);

        [[nodiscard]] std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] const std::string &GetName() const;
        void SetName(std::string name);

    protected:
        std::string m_Name;
    };

    class Variable final : public Global
    {
    public:
        explicit Variable(Type *type, std::string name, Constant *initializer);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Constant *m_Initializer;
    };
}

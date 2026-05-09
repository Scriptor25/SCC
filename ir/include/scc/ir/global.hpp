#pragma once

#include <scc/ir/constant.hpp>

#include <iosfwd>
#include <string>

namespace scc::ir
{
    class Global : public Constant
    {
    public:
        explicit Global(Type *type, std::string name);

        [[nodiscard]] std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        [[nodiscard]] bool Compare(Constant *value) const override;

        void SetName(std::string name);
        [[nodiscard]] const std::string &GetName() const;

    protected:
        std::string m_Name;
    };
}

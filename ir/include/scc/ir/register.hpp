#pragma once

#include <scc/ir/ir.hpp>

#include <string>

namespace scc::ir
{
    class Register
    {
    public:
        explicit Register(std::string name = {});

        void SetValue(ValueFwd::WeakPtr value);
        void ClearValue();

        [[nodiscard]] std::string GetName() const;
        void SetName(std::string name);

        [[nodiscard]] ValueFwd::Ptr GetValue() const;

        std::ostream &Print(std::ostream &stream) const;

    private:
        std::string m_Name;
        ValueFwd::WeakPtr m_Value;
    };
}

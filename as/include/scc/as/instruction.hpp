#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>
#include <scc/as/operand.hpp>

#include <vector>

namespace scc::as
{
    class Instruction : public Fragment
    {
    public:
        Instruction(InstructionCode code, std::vector<Operand> operands);

        void SetCode(InstructionCode code);

        [[nodiscard]] InstructionCode GetCode() const;

        [[nodiscard]] size_t GetOperandCount() const;

        Operand &operator[](size_t index);
        [[nodiscard]] const Operand &operator[](size_t index) const;

    private:
        InstructionCode m_Code;
        std::vector<Operand> m_Operands;
    };
}

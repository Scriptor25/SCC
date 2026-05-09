#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>
#include <scc/as/operand.hpp>

#include <vector>

namespace scc::as
{
    class Instruction : public Fragment
    {
        template<bool constant>
        struct iterator
        {
            using value_type = std::conditional_t<constant, const Operand, Operand>;
            using collection_type = std::conditional_t<constant, const std::vector<Operand>, std::vector<Operand>>;

            bool operator!=(const iterator &other) const
            {
                return index != other.index;
            }

            value_type &operator*() const
            {
                return operands[index];
            }

            iterator &operator++()
            {
                if (index == ~size_t())
                    return *this;

                if (++index >= operands.size())
                    index = ~size_t();

                return *this;
            }

            size_t index;
            collection_type &operands;
        };

    public:
        explicit Instruction(InstructionCode code, std::vector<Operand> operands);

        void SetCode(InstructionCode code);

        [[nodiscard]] InstructionCode GetCode() const;

        [[nodiscard]] size_t GetOperandCount() const;

        Operand &operator[](size_t index);

        [[nodiscard]] const Operand &operator[](size_t index) const;

        iterator<false> begin();
        iterator<false> end();

        [[nodiscard]] iterator<true> begin() const;
        [[nodiscard]] iterator<true> end() const;

    private:
        InstructionCode m_Code;
        std::vector<Operand> m_Operands;
    };
}

#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>
#include <scc/as/operand.hpp>

#include <scc/common.hpp>

#include <memory>
#include <vector>

namespace scc::as
{
    class Instruction : public Fragment
    {
        template<bool constant>
        struct iterator
        {
            using value_type = std::conditional_t<constant, const Operand, Operand>;
            using collection_type = std::conditional_t<
                constant,
                const std::vector<std::unique_ptr<Operand>>,
                std::vector<std::unique_ptr<Operand>>
            >;

            bool operator!=(const iterator &other) const
            {
                return index != other.index;
            }

            value_type &operator*() const
            {
                return *operands[index];
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
        explicit Instruction(Mnemonic mnemonic, std::vector<std::unique_ptr<Operand>> operands = {});

        void SetMnemonic(Mnemonic mnemonic);

        [[nodiscard]] Mnemonic GetMnemonic() const;

        [[nodiscard]] size_t GetOperandCount() const;

        Operand &operator[](size_t index);

        [[nodiscard]] const Operand &operator[](size_t index) const;

        iterator<false> begin();
        iterator<false> end();

        [[nodiscard]] iterator<true> begin() const;
        [[nodiscard]] iterator<true> end() const;

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Mnemonic m_Mnemonic;
        std::vector<std::unique_ptr<Operand>> m_Operands;
    };
}

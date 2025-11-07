#pragma once

#include <vector>
#include <scc/ir/block.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Block final : public NamedValue, public Shared<Block>
    {
    public:
        explicit Block(std::string name, Function::WeakPtr function);

        std::ostream &Print(std::ostream &stream) const override;
        std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] Function::Ptr GetFunction() const;
        [[nodiscard]] unsigned GetInstructionCount() const;
        [[nodiscard]] InstructionPtr GetInstruction(unsigned index) const;

        [[nodiscard]] std::vector<InstructionPtr>::const_iterator begin() const;
        [[nodiscard]] std::vector<InstructionPtr>::const_iterator end() const;

        void Insert(InstructionPtr instruction);

    private:
        Function::WeakPtr m_Function;
        std::vector<InstructionPtr> m_Instructions;
    };
}

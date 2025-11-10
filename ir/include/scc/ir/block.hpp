#pragma once

#include <vector>
#include <scc/ir/block.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Block final : public Shared<Block>
    {
    public:
        explicit Block(std::string name, Function::WeakPtr function);

        std::string GetName() const;
        void SetName(std::string name);

        std::ostream &Print(std::ostream &stream) const;
        std::ostream &PrintOperand(std::ostream &stream) const;

        [[nodiscard]] Function::Ptr GetFunction() const;
        [[nodiscard]] unsigned GetInstructionCount() const;
        [[nodiscard]] InstructionFwd::Ptr GetInstruction(unsigned index) const;

        [[nodiscard]] std::vector<InstructionFwd::Ptr>::const_iterator begin() const;
        [[nodiscard]] std::vector<InstructionFwd::Ptr>::const_iterator end() const;

        void Insert(InstructionFwd::Ptr instruction);

        RegisterFwd::Ptr CreateRegister(std::string name = {}) const;

    private:
        std::string m_Name;
        Function::WeakPtr m_Function;
        std::vector<InstructionFwd::Ptr> m_Instructions;
    };
}

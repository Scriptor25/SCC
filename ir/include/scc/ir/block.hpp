#pragma once

#include <vector>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Block final : public Shared<Block>
    {
    public:
        explicit Block(Function::WeakPtr function, std::string name);

        std::ostream &Print(std::ostream &stream) const;

        [[nodiscard]] Function::Ptr GetFunction() const;
        [[nodiscard]] std::string GetName() const;
        [[nodiscard]] unsigned GetInstructionCount() const;
        [[nodiscard]] InstructionPtr GetInstruction(unsigned index) const;

        [[nodiscard]] std::vector<InstructionPtr>::const_iterator begin() const;
        [[nodiscard]] std::vector<InstructionPtr>::const_iterator end() const;

    private:
        Function::WeakPtr m_Function;
        std::string m_Name;
        std::vector<InstructionPtr> m_Instructions;
    };
}

#pragma once

#include <scc/ir/instruction.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace scc::ir
{
    class Block final : public Value
    {
    public:
        explicit Block(std::string name, Function *function);

        void DropAll();

        void SetName(std::string name);
        [[nodiscard]] const std::string &GetName() const;

        std::ostream &Print(std::ostream &stream) const override;
        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        [[nodiscard]] Function *GetFunction() const;

        [[nodiscard]] size_t GetInstructionCount() const;
        [[nodiscard]] Instruction *GetInstruction(size_t index) const;

        Instruction *Insert(std::unique_ptr<Instruction> instruction);
        void Erase(const Instruction *instruction);

        Value *CreateEmpty(Type *type, std::string name) const;

        [[nodiscard]] Value *FindValue(const std::string &name) const;

        [[nodiscard]] Instruction *GetTerminator() const;

        [[nodiscard]] std::unordered_set<Block *> GetPredecessors() const;
        [[nodiscard]] std::unordered_set<Block *> GetSuccessors() const;

    private:
        std::string m_Name;
        Function *m_Function;

        std::vector<std::unique_ptr<Instruction>> m_Instructions;
    };
}

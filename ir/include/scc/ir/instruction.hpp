#pragma once

#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Instruction : public Value
    {
    public:
        explicit Instruction(TypePtr type, std::string name);

    protected:
        std::string m_Name;
    };

    enum Operator
    {
        Operator_Add,
        Operator_Sub,
        Operator_Mul,
        Operator_Div,
        Operator_Rem,
        Operator_And,
        Operator_Or,
        Operator_Xor,
    };

    class OperatorInstruction final : public Instruction
    {
    public:
        explicit OperatorInstruction(
            TypePtr type,
            std::string name,
            Operator operator_,
            std::vector<ValuePtr> operands);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Operator m_Operator;
        std::vector<ValuePtr> m_Operands;
    };

    enum Comparator
    {
        Comparator_LT,
        Comparator_GT,
        Comparator_LE,
        Comparator_GE,
        Comparator_EQ,
        Comparator_NE,
    };

    class ComparatorInstruction final : public Instruction
    {
    public:
        explicit ComparatorInstruction(
            TypePtr type,
            std::string name,
            Comparator comparator,
            ValuePtr lhs,
            ValuePtr rhs);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Comparator m_Comparator;
        ValuePtr m_LHS, m_RHS;
    };

    class BranchInstruction final : public Instruction
    {
    public:
        explicit BranchInstruction(BlockPtr destination);
        explicit BranchInstruction(ValuePtr condition, BlockPtr then, BlockPtr else_);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Condition;
        BlockPtr m_Then, m_Else;
    };
}

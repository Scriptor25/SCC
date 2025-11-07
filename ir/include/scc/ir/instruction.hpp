#pragma once

#include <scc/ir/block.hpp>
#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Instruction
    {
    public:
        explicit Instruction(Block::WeakPtr block);
        virtual ~Instruction() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

    protected:
        Block::WeakPtr m_Block;
    };

    class NamedInstruction : public Instruction, public NamedValue
    {
    public:
        NamedInstruction(TypePtr type, std::string name, Block::WeakPtr block);

        std::ostream &PrintOperand(std::ostream &stream) const override;
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

    class OperatorInstruction final : public NamedInstruction, public Shared<OperatorInstruction>
    {
    public:
        explicit OperatorInstruction(
            TypePtr type,
            std::string name,
            Block::WeakPtr block,
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

    class ComparatorInstruction final : public NamedInstruction, public Shared<ComparatorInstruction>
    {
    public:
        explicit ComparatorInstruction(
            TypePtr type,
            std::string name,
            Block::WeakPtr block,
            Comparator comparator,
            ValuePtr lhs,
            ValuePtr rhs);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Comparator m_Comparator;
        ValuePtr m_LHS, m_RHS;
    };

    class BranchInstruction final : public Instruction, public Shared<BranchInstruction>
    {
    public:
        explicit BranchInstruction(Block::WeakPtr block, BlockPtr destination);
        explicit BranchInstruction(Block::WeakPtr block, ValuePtr condition, BlockPtr then, BlockPtr else_);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Condition;
        BlockPtr m_Then, m_Else;
    };

    class RetInstruction final : public Instruction, public Shared<RetInstruction>
    {
    public:
        explicit RetInstruction(Block::WeakPtr block);
        explicit RetInstruction(Block::WeakPtr block, ValuePtr value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Value;
    };

    class SelectInstruction final : public NamedInstruction, public Shared<SelectInstruction>
    {
    public:
        explicit SelectInstruction(
            TypePtr type,
            std::string name,
            Block::WeakPtr block,
            std::vector<std::pair<BlockPtr, ValuePtr>> options);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<std::pair<BlockPtr, ValuePtr>> m_Options;
    };

    class AllocInstruction final : public NamedInstruction, public Shared<AllocInstruction>
    {
    public:
        explicit AllocInstruction(TypePtr type, std::string name, Block::WeakPtr block);

        std::ostream &Print(std::ostream &stream) const override;
    };

    class LoadInstruction final : public NamedInstruction, public Shared<LoadInstruction>
    {
    public:
        explicit LoadInstruction(TypePtr type, std::string name, Block::WeakPtr block, ValuePtr pointer);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Pointer;
    };

    class StoreInstruction final : public Instruction, public Shared<StoreInstruction>
    {
    public:
        explicit StoreInstruction(Block::WeakPtr block, ValuePtr pointer, ValuePtr value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Pointer;
        ValuePtr m_Value;
    };

    class OffsetInstruction final : public NamedInstruction, public Shared<OffsetInstruction>
    {
    public:
        explicit OffsetInstruction(
            TypePtr type,
            std::string name,
            Block::WeakPtr block,
            ValuePtr base,
            std::vector<ValuePtr> offsets);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Base;
        std::vector<ValuePtr> m_Offsets;
    };

    class CallInstruction final : public NamedInstruction, public Shared<CallInstruction>
    {
    public:
        explicit CallInstruction(
            TypePtr type,
            std::string name,
            Block::WeakPtr block,
            ValuePtr callee,
            std::vector<ValuePtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Callee;
        std::vector<ValuePtr> m_Arguments;
    };

    class CallVoidInstruction final : public Instruction, public Shared<CallVoidInstruction>
    {
    public:
        explicit CallVoidInstruction(Block::WeakPtr block, ValuePtr callee, std::vector<ValuePtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Callee;
        std::vector<ValuePtr> m_Arguments;
    };

    class CastInstruction final : public NamedInstruction, public Shared<CastInstruction>
    {
    public:
        explicit CastInstruction(
            TypePtr type,
            std::string name,
            Block::WeakPtr block,
            ValuePtr value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ValuePtr m_Value;
    };
}

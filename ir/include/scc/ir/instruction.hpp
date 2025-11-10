#pragma once

#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Instruction
    {
    public:
        explicit Instruction(BlockFwd::WeakPtr block);
        virtual ~Instruction() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

        [[nodiscard]] BlockFwd::Ptr GetBlock() const;

    protected:
        BlockFwd::WeakPtr m_Block;
    };

    class IdentifiedInstruction : public Instruction, public IdentifiedValue
    {
    public:
        explicit IdentifiedInstruction(TypeFwd::Ptr type, RegisterFwd::Ptr register_, BlockFwd::WeakPtr block);
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

    class OperatorInstruction final : public IdentifiedInstruction, public Shared<OperatorInstruction>
    {
    public:
        explicit OperatorInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            Operator operator_,
            std::vector<ValueFwd::Ptr> operands);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Operator GetOperator() const;
        [[nodiscard]] unsigned GetOperandCount() const;
        [[nodiscard]] ValueFwd::Ptr GetOperand(unsigned index) const;

    private:
        Operator m_Operator;
        std::vector<ValueFwd::Ptr> m_Operands;
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

    class ComparatorInstruction final : public IdentifiedInstruction, public Shared<ComparatorInstruction>
    {
    public:
        explicit ComparatorInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            Comparator comparator,
            ValueFwd::Ptr lhs,
            ValueFwd::Ptr rhs);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Comparator GetComparator() const;
        [[nodiscard]] ValueFwd::Ptr GetLHS() const;
        [[nodiscard]] ValueFwd::Ptr GetRHS() const;

    private:
        Comparator m_Comparator;
        ValueFwd::Ptr m_LHS, m_RHS;
    };

    class BranchInstruction final : public Instruction, public Shared<BranchInstruction>
    {
    public:
        explicit BranchInstruction(BlockFwd::WeakPtr block, BlockFwd::Ptr destination);
        explicit BranchInstruction(
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr condition,
            BlockFwd::Ptr then,
            BlockFwd::Ptr else_);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetCondition() const;
        [[nodiscard]] BlockFwd::Ptr GetThen() const;
        [[nodiscard]] BlockFwd::Ptr GetElse() const;

    private:
        ValueFwd::Ptr m_Condition;
        BlockFwd::Ptr m_Then, m_Else;
    };

    class RetInstruction final : public Instruction, public Shared<RetInstruction>
    {
    public:
        explicit RetInstruction(BlockFwd::WeakPtr block);
        explicit RetInstruction(BlockFwd::WeakPtr block, ValueFwd::Ptr value);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetValue() const;

    private:
        ValueFwd::Ptr m_Value;
    };

    class SelectInstruction final : public IdentifiedInstruction, public Shared<SelectInstruction>
    {
    public:
        explicit SelectInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> options);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetOptionCount() const;
        [[nodiscard]] std::pair<BlockFwd::Ptr, ValueFwd::Ptr> GetOption(unsigned index) const;

    private:
        std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> m_Options;
    };

    class AllocInstruction final : public IdentifiedInstruction, public Shared<AllocInstruction>
    {
    public:
        explicit AllocInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            unsigned count);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetCount() const;

    private:
        unsigned m_Count;
    };

    class LoadInstruction final : public IdentifiedInstruction, public Shared<LoadInstruction>
    {
    public:
        explicit LoadInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr pointer);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetPointer() const;

    private:
        ValueFwd::Ptr m_Pointer;
    };

    class StoreInstruction final : public Instruction, public Shared<StoreInstruction>
    {
    public:
        explicit StoreInstruction(BlockFwd::WeakPtr block, ValueFwd::Ptr pointer, ValueFwd::Ptr value);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetPointer() const;
        [[nodiscard]] ValueFwd::Ptr GetValue() const;

    private:
        ValueFwd::Ptr m_Pointer;
        ValueFwd::Ptr m_Value;
    };

    class OffsetInstruction final : public IdentifiedInstruction, public Shared<OffsetInstruction>
    {
    public:
        explicit OffsetInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr base,
            std::vector<ValueFwd::Ptr> offsets);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetBase() const;
        [[nodiscard]] unsigned GetOffsetCount() const;
        [[nodiscard]] ValueFwd::Ptr GetOffset(unsigned index) const;

    private:
        ValueFwd::Ptr m_Base;
        std::vector<ValueFwd::Ptr> m_Offsets;
    };

    class CallInstruction final : public IdentifiedInstruction, public Shared<CallInstruction>
    {
    public:
        explicit CallInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr callee,
            std::vector<ValueFwd::Ptr> arguments);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetCallee() const;
        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] ValueFwd::Ptr GetArgument(unsigned index) const;

    private:
        ValueFwd::Ptr m_Callee;
        std::vector<ValueFwd::Ptr> m_Arguments;
    };

    class CallVoidInstruction final : public Instruction, public Shared<CallVoidInstruction>
    {
    public:
        explicit CallVoidInstruction(
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr callee,
            std::vector<ValueFwd::Ptr> arguments);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetCallee() const;
        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] ValueFwd::Ptr GetArgument(unsigned index) const;

    private:
        ValueFwd::Ptr m_Callee;
        std::vector<ValueFwd::Ptr> m_Arguments;
    };

    class CastInstruction final : public IdentifiedInstruction, public Shared<CastInstruction>
    {
    public:
        explicit CastInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr value);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetValue() const;

    private:
        ValueFwd::Ptr m_Value;
    };
}

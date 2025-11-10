#pragma once

#include <scc/ir/value.hpp>

namespace scc::ir
{
    class Instruction : public IdentifiedValue
    {
    public:
        explicit Instruction(TypeFwd::Ptr type, RegisterFwd::Ptr register_, BlockFwd::WeakPtr block);

        [[nodiscard]] BlockFwd::Ptr GetBlock() const;

    protected:
        BlockFwd::WeakPtr m_Block;
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

    class OperatorInstruction final : public Instruction, public Shared<OperatorInstruction>
    {
    public:
        explicit OperatorInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            Operator operator_,
            std::vector<ValueFwd::Ptr> operands);
        ~OperatorInstruction() override;

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

    class ComparatorInstruction final : public Instruction, public Shared<ComparatorInstruction>
    {
    public:
        explicit ComparatorInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            Comparator comparator,
            ValueFwd::Ptr lhs,
            ValueFwd::Ptr rhs);
        ~ComparatorInstruction() override;

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
        explicit BranchInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            BlockFwd::Ptr destination);
        explicit BranchInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr condition,
            BlockFwd::Ptr then,
            BlockFwd::Ptr else_);
        ~BranchInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetCondition() const;
        [[nodiscard]] BlockFwd::Ptr GetThen() const;
        [[nodiscard]] BlockFwd::Ptr GetElse() const;

    private:
        ValueFwd::Ptr m_Condition;
        BlockFwd::Ptr m_Then, m_Else;
    };

    class ReturnInstruction final : public Instruction, public Shared<ReturnInstruction>
    {
    public:
        explicit ReturnInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr value);
        ~ReturnInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetValue() const;

    private:
        ValueFwd::Ptr m_Value;
    };

    class SelectInstruction final : public Instruction, public Shared<SelectInstruction>
    {
    public:
        explicit SelectInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> options);
        ~SelectInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetOptionCount() const;
        [[nodiscard]] std::pair<BlockFwd::Ptr, ValueFwd::Ptr> GetOption(unsigned index) const;

    private:
        std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> m_Options;
    };

    class AllocInstruction final : public Instruction, public Shared<AllocInstruction>
    {
    public:
        explicit AllocInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            unsigned count);
        ~AllocInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetCount() const;

    private:
        unsigned m_Count;
    };

    class LoadInstruction final : public Instruction, public Shared<LoadInstruction>
    {
    public:
        explicit LoadInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr pointer);
        ~LoadInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetPointer() const;

    private:
        ValueFwd::Ptr m_Pointer;
    };

    class StoreInstruction final : public Instruction, public Shared<StoreInstruction>
    {
    public:
        explicit StoreInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr pointer,
            ValueFwd::Ptr value);
        ~StoreInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetPointer() const;
        [[nodiscard]] ValueFwd::Ptr GetValue() const;

    private:
        ValueFwd::Ptr m_Pointer;
        ValueFwd::Ptr m_Value;
    };

    class OffsetInstruction final : public Instruction, public Shared<OffsetInstruction>
    {
    public:
        explicit OffsetInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr base,
            std::vector<ValueFwd::Ptr> offsets);
        ~OffsetInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetBase() const;
        [[nodiscard]] unsigned GetOffsetCount() const;
        [[nodiscard]] ValueFwd::Ptr GetOffset(unsigned index) const;

    private:
        ValueFwd::Ptr m_Base;
        std::vector<ValueFwd::Ptr> m_Offsets;
    };

    class CallInstruction final : public Instruction, public Shared<CallInstruction>
    {
    public:
        explicit CallInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr callee,
            std::vector<ValueFwd::Ptr> arguments);
        ~CallInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetCallee() const;
        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] ValueFwd::Ptr GetArgument(unsigned index) const;

    private:
        ValueFwd::Ptr m_Callee;
        std::vector<ValueFwd::Ptr> m_Arguments;
    };

    class CastInstruction final : public Instruction, public Shared<CastInstruction>
    {
    public:
        explicit CastInstruction(
            TypeFwd::Ptr type,
            RegisterFwd::Ptr register_,
            BlockFwd::WeakPtr block,
            ValueFwd::Ptr value);
        ~CastInstruction() override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ValueFwd::Ptr GetValue() const;

    private:
        ValueFwd::Ptr m_Value;
    };
}

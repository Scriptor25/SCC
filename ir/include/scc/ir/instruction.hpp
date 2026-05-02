#pragma once

#include <scc/ir/user.hpp>

#include <string>
#include <vector>

namespace scc::ir
{
    class Instruction : public User
    {
    public:
        explicit Instruction(Type *type, Block *block);
        explicit Instruction(Type *type, Block *block, std::string name);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] Block *GetBlock() const;

        void SetName(std::string name);
        [[nodiscard]] const std::string &GetName() const;

        [[nodiscard]] virtual bool IsTerminator() const;
        [[nodiscard]] virtual size_t GetSuccessorCount() const;
        [[nodiscard]] virtual Block *GetSuccessor(size_t index) const;

    protected:
        Block *m_Block;
        std::string m_Name;
    };

    enum class Operator
    {
        Add,
        Sub,
        Mul,
        SDiv,
        UDiv,
        SRem,
        URem,
        And,
        Or,
        Xor,
    };

    class OperatorInstruction final : public Instruction
    {
    public:
        explicit OperatorInstruction(
            Type *type,
            Block *block,
            std::string name,
            Operator operator_,
            std::vector<Value *> operands);
        ~OperatorInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Operator GetOperator() const;
        [[nodiscard]] unsigned GetOperandCount() const;
        [[nodiscard]] Value *GetOperand(unsigned index) const;

    private:
        Operator m_Operator;
        std::vector<Value *> m_Operands;
    };

    enum class Comparator
    {
        SLT,
        ULT,
        SGT,
        UGT,
        SLE,
        ULE,
        SGE,
        UGE,
        EQ,
        NE,
    };

    class ComparatorInstruction final : public Instruction
    {
    public:
        explicit ComparatorInstruction(
            Type *type,
            Block *block,
            std::string name,
            Comparator comparator,
            Value *lhs,
            Value *rhs);
        ~ComparatorInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Comparator GetComparator() const;
        [[nodiscard]] Value *GetLHS() const;
        [[nodiscard]] Value *GetRHS() const;

    private:
        Comparator m_Comparator;
        Value *m_LHS, *m_RHS;
    };

    class DirectBranchInstruction final : public Instruction
    {
    public:
        explicit DirectBranchInstruction(
            Type *type,
            Block *block,
            Block *destination);
        ~DirectBranchInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        bool IsTerminator() const override;
        size_t GetSuccessorCount() const override;
        Block *GetSuccessor(size_t index) const override;

        [[nodiscard]] Block *GetDestination() const;

    private:
        Block *m_Destination;
    };

    class BranchInstruction final : public Instruction
    {
    public:
        explicit BranchInstruction(
            Type *type,
            Block *block,
            Value *condition,
            Block *then,
            Block *else_);
        ~BranchInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        bool IsTerminator() const override;
        size_t GetSuccessorCount() const override;
        Block *GetSuccessor(size_t index) const override;

        [[nodiscard]] Value *GetCondition() const;
        [[nodiscard]] Block *GetThen() const;
        [[nodiscard]] Block *GetElse() const;

    private:
        Value *m_Condition;
        Block *m_Then, *m_Else;
    };

    class ReturnInstruction final : public Instruction
    {
    public:
        explicit ReturnInstruction(
            Type *type,
            Block *block);
        explicit ReturnInstruction(
            Type *type,
            Block *block,
            Value *value);
        ~ReturnInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        bool IsTerminator() const override;

        [[nodiscard]] Value *GetValue() const;

    private:
        Value *m_Value;
    };

    class SelectInstruction final : public Instruction
    {
    public:
        explicit SelectInstruction(
            Type *type,
            Block *block,
            std::string name,
            std::vector<std::pair<Block *, Value *>> options);
        ~SelectInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetOptionCount() const;
        [[nodiscard]] std::pair<Block *, Value *> GetOption(unsigned index) const;

    private:
        std::vector<std::pair<Block *, Value *>> m_Options;
    };

    class AllocInstruction final : public Instruction
    {
    public:
        explicit AllocInstruction(
            PointerType *type,
            Block *block,
            std::string name,
            unsigned count);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetCount() const;

    private:
        unsigned m_Count;
    };

    class LoadInstruction final : public Instruction
    {
    public:
        explicit LoadInstruction(
            Type *type,
            Block *block,
            std::string name,
            Value *pointer);
        ~LoadInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Value *GetPointer() const;

    private:
        Value *m_Pointer;
    };

    class StoreInstruction final : public Instruction
    {
    public:
        explicit StoreInstruction(
            Type *type,
            Block *block,
            Value *pointer,
            Value *value);
        ~StoreInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Value *GetPointer() const;
        [[nodiscard]] Value *GetValue() const;

    private:
        Value *m_Pointer;
        Value *m_Value;
    };

    class OffsetInstruction final : public Instruction
    {
    public:
        explicit OffsetInstruction(
            Type *type,
            Block *block,
            std::string name,
            Value *base,
            std::vector<Value *> offsets);
        ~OffsetInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Value *GetBase() const;
        [[nodiscard]] unsigned GetOffsetCount() const;
        [[nodiscard]] Value *GetOffset(unsigned index) const;

    private:
        Value *m_Base;
        std::vector<Value *> m_Offsets;
    };

    class CallInstruction final : public Instruction
    {
    public:
        explicit CallInstruction(
            Type *type,
            Block *block,
            std::string name,
            Value *callee,
            std::vector<Value *> arguments);
        ~CallInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Value *GetCallee() const;
        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] Value *GetArgument(unsigned index) const;

    private:
        Value *m_Callee;
        std::vector<Value *> m_Arguments;
    };

    class CastInstruction final : public Instruction
    {
    public:
        explicit CastInstruction(
            Type *type,
            Block *block,
            std::string name,
            Value *value);
        ~CastInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Value *GetValue() const;

    private:
        Value *m_Value;
    };
}

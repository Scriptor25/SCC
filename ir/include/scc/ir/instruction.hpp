#pragma once

#include <scc/ir/ir.hpp>
#include <scc/ir/user.hpp>

#include <iosfwd>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace scc::ir
{
    enum class IOperator
    {
        ADD,
        SUB,
        MUL,
        SDIV,
        UDIV,
        SREM,
        UREM,
        AND,
        OR,
        XOR,
    };

    extern const std::unordered_map<IOperator, std::string_view> IOperatorToString;
    extern const std::unordered_map<std::string_view, IOperator> StringToIOperator;

    enum class FOperator
    {
        ADD,
        SUB,
        MUL,
        DIV,
        REM,
    };

    extern const std::unordered_map<FOperator, std::string_view> FOperatorToString;
    extern const std::unordered_map<std::string_view, FOperator> StringToFOperator;

    enum class ICompare
    {
        SLT,
        ULT,
        SGT,
        UGT,
        SLE,
        ULE,
        SGE,
        UGE,
        EQU,
        NEQ,
    };

    extern const std::unordered_map<ICompare, std::string_view> ICompareToString;
    extern const std::unordered_map<std::string_view, ICompare> StringToICompare;

    enum class FCompare
    {
        OLT,
        ULT,
        OGT,
        UGT,
        OLE,
        ULE,
        OGE,
        UGE,
        OEQ,
        UEQ,
        ONE,
        UNE,
    };

    extern const std::unordered_map<FCompare, std::string_view> FCompareToString;
    extern const std::unordered_map<std::string_view, FCompare> StringToFCompare;

    class Instruction : public User
    {
    public:
        explicit Instruction(Type *type, Block *block);
        explicit Instruction(Type *type, Block *block, std::string name);

        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

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

    class IOperatorInstruction : public Instruction
    {
    public:
        explicit IOperatorInstruction(
            Type *type,
            Block *block,
            std::string name,
            IOperator operator_,
            std::vector<Value *> operands);
        ~IOperatorInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] IOperator GetOperator() const;
        [[nodiscard]] unsigned GetOperandCount() const;
        [[nodiscard]] Value *GetOperand(unsigned index) const;

    private:
        IOperator m_Operator;
        std::vector<Value *> m_Operands;
    };

    class FOperatorInstruction : public Instruction
    {
    public:
        explicit FOperatorInstruction(
            Type *type,
            Block *block,
            std::string name,
            FOperator operator_,
            std::vector<Value *> operands);
        ~FOperatorInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] FOperator GetOperator() const;
        [[nodiscard]] unsigned GetOperandCount() const;
        [[nodiscard]] Value *GetOperand(unsigned index) const;

    private:
        FOperator m_Operator;
        std::vector<Value *> m_Operands;
    };

    class ICompareInstruction : public Instruction
    {
    public:
        explicit ICompareInstruction(
            Type *type,
            Block *block,
            std::string name,
            ICompare compare,
            Value *lhs,
            Value *rhs);
        ~ICompareInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] ICompare GetCompare() const;
        [[nodiscard]] Value *GetLHS() const;
        [[nodiscard]] Value *GetRHS() const;

    private:
        ICompare m_Compare;
        Value *m_LHS, *m_RHS;
    };

    class FCompareInstruction : public Instruction
    {
    public:
        explicit FCompareInstruction(
            Type *type,
            Block *block,
            std::string name,
            FCompare compare,
            Value *lhs,
            Value *rhs);
        ~FCompareInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] FCompare GetCompare() const;
        [[nodiscard]] Value *GetLHS() const;
        [[nodiscard]] Value *GetRHS() const;

    private:
        FCompare m_Compare;
        Value *m_LHS, *m_RHS;
    };

    class DirectBranchInstruction : public Instruction
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

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        bool IsTerminator() const override;
        size_t GetSuccessorCount() const override;
        Block *GetSuccessor(size_t index) const override;

        [[nodiscard]] Block *GetDestination() const;

    private:
        Block *m_Destination;
    };

    class BranchInstruction : public Instruction
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

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

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

    class ReturnInstruction : public Instruction
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

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        bool IsTerminator() const override;

        [[nodiscard]] Value *GetValue() const;

    private:
        Value *m_Value;
    };

    class PhiInstruction : public Instruction
    {
    public:
        explicit PhiInstruction(
            Type *type,
            Block *block,
            std::string name,
            std::vector<std::pair<Block *, Value *>> nodes);
        ~PhiInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] size_t GetNodeCount() const;
        std::pair<Block *, Value *> &GetNode(size_t index);
        [[nodiscard]] const std::pair<Block *, Value *> &GetNode(size_t index) const;

    private:
        std::vector<std::pair<Block *, Value *>> m_Nodes;
    };

    class AllocInstruction : public Instruction
    {
    public:
        explicit AllocInstruction(
            PointerType *type,
            Block *block,
            std::string name,
            uint64_t count);

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] uint64_t GetCount() const;

    private:
        uint64_t m_Count;
    };

    class LoadInstruction : public Instruction
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

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] Value *GetPointer() const;

    private:
        Value *m_Pointer;
    };

    class StoreInstruction : public Instruction
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

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] Value *GetPointer() const;
        [[nodiscard]] Value *GetValue() const;

    private:
        Value *m_Pointer;
        Value *m_Value;
    };

    class ElementPointerInstruction : public Instruction
    {
    public:
        explicit ElementPointerInstruction(
            Type *type,
            Block *block,
            std::string name,
            Value *pointer,
            std::vector<Value *> indices);
        ~ElementPointerInstruction() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &Print(std::ostream &stream) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] Value *GetBase() const;
        [[nodiscard]] size_t GetIndexCount() const;
        [[nodiscard]] Value *GetIndex(size_t index) const;

    private:
        Value *m_Pointer;
        std::vector<Value *> m_Indices;
    };

    class CallInstruction : public Instruction
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

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] Value *GetCallee() const;
        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] Value *GetArgument(unsigned index) const;

    private:
        Value *m_Callee;
        std::vector<Value *> m_Arguments;
    };

    class CastInstruction : public Instruction
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

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;

        [[nodiscard]] Value *GetValue() const;

    private:
        Value *m_Value;
    };
}

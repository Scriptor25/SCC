#pragma once

#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

#include <scc/assert.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace scc::ir
{
    class Builder
    {
    public:
        explicit Builder(Context &context);

        [[nodiscard]] Context &GetContext() const;

        [[nodiscard]] Variable *CreateString(
            Module &module,
            std::string name,
            std::string_view value) const;

        [[nodiscard]] Block *GetOrCreateBlock(
            Function *function,
            std::string name) const;

        void SetInsertBlock(Block *block);
        void ClearInsertBlock();

        [[nodiscard]] Block *GetInsertBlock() const;
        [[nodiscard]] Function *GetInsertFunction() const;
        [[nodiscard]] Type *GetInsertFunctionResult() const;

        Value *CreateEmpty(Type *type, std::string name = {}) const;

        IOperatorInstruction *CreateIOperator(
            IOperator operator_,
            Type *type,
            std::vector<Value *> operands,
            std::string name = {});

        IOperatorInstruction *CreateIOperatorADD(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorSUB(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorMUL(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorSDIV(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorUDIV(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorSREM(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorUREM(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorAND(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorOR(Type *type, std::vector<Value *> operands, std::string name = {});
        IOperatorInstruction *CreateIOperatorXOR(Type *type, std::vector<Value *> operands, std::string name = {});

        FOperatorInstruction *CreateFOperator(
            FOperator operator_,
            Type *type,
            std::vector<Value *> operands,
            std::string name = {});

        FOperatorInstruction *CreateFOperatorADD(Type *type, std::vector<Value *> operands, std::string name = {});
        FOperatorInstruction *CreateFOperatorSUB(Type *type, std::vector<Value *> operands, std::string name = {});
        FOperatorInstruction *CreateFOperatorMUL(Type *type, std::vector<Value *> operands, std::string name = {});
        FOperatorInstruction *CreateFOperatorDIV(Type *type, std::vector<Value *> operands, std::string name = {});
        FOperatorInstruction *CreateFOperatorREM(Type *type, std::vector<Value *> operands, std::string name = {});

        ICompareInstruction *CreateICompare(
            ICompare comparator,
            Type *type,
            Value *lhs,
            Value *rhs,
            std::string name = {});

        ICompareInstruction *CreateICompareSLT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareULT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareSGT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareUGT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareSLE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareULE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareSGE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareUGE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareEQU(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ICompareInstruction *CreateICompareNEQ(Type *type, Value *lhs, Value *rhs, std::string name = {});

        FCompareInstruction *CreateFCompare(
            FCompare comparator,
            Type *type,
            Value *lhs,
            Value *rhs,
            std::string name = {});

        FCompareInstruction *CreateFCompareOLT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareULT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareOGT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareUGT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareOLE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareULE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareOGE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareUGE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareOEQ(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareUEQ(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareONE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        FCompareInstruction *CreateFCompareUNE(Type *type, Value *lhs, Value *rhs, std::string name = {});

        DirectBranchInstruction *CreateBranch(Block *destination);
        BranchInstruction *CreateBranch(
            Value *condition,
            Block *then,
            Block *else_);

        ReturnInstruction *CreateReturn();
        ReturnInstruction *CreateReturn(Value *value);

        PhiInstruction *CreatePhi(
            Type *type,
            std::vector<std::pair<Block *, Value *>> nodes,
            std::string name = {});

        AllocInstruction *CreateAlloc(
            Type *type,
            uint64_t count = 1,
            std::string name = {});
        LoadInstruction *CreateLoad(
            Value *pointer,
            std::string name = {});
        StoreInstruction *CreateStore(
            Value *pointer,
            Value *value);

        ElementPointerInstruction *CreateElementPointer(
            Value *pointer,
            std::vector<Value *> indices,
            std::string name = {});

        ElementPointerInstruction *CreateElementPointer(
            Value *pointer,
            const std::vector<size_t> &indices,
            std::string name = {});

        CallInstruction *CreateCall(
            FunctionType *function_type,
            Value *callee,
            std::vector<Value *> arguments,
            std::string name = {});

        CastInstruction *CreateCast(
            Type *type,
            Value *value,
            std::string name = {});

        template<std::derived_from<Instruction> T, typename... Args>
        [[nodiscard]] T *Create(Args &&... args) const
        {
            Assert(m_InsertBlock, "insert block must not be null");

            auto instruction = std::make_unique<T>(std::forward<Args>(args)...);
            auto *ptr = instruction.get();

            m_InsertBlock->Insert(std::move(instruction));

            return ptr;
        }

    private:
        Context &m_Context;
        Block *m_InsertBlock;
    };
}

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
    class Builder final
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

        OperatorInstruction *CreateOperator(
            Operator operator_,
            Type *type,
            std::vector<Value *> operands,
            std::string name = {});

        OperatorInstruction *CreateAdd(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateSub(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateMul(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateSDiv(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateUDiv(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateSRem(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateURem(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateAnd(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateOr(Type *type, std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateXor(Type *type, std::vector<Value *> operands, std::string name = {});

        ComparatorInstruction *CreateComparator(
            Comparator comparator,
            Type *type,
            Value *lhs,
            Value *rhs,
            std::string name = {});

        ComparatorInstruction *CreateSLT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateULT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateSGT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateUGT(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateSLE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateULE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateSGE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateUGE(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateEQ(Type *type, Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateNE(Type *type, Value *lhs, Value *rhs, std::string name = {});

        DirectBranchInstruction *CreateBranch(Block *destination);
        BranchInstruction *CreateBranch(
            Value *condition,
            Block *then,
            Block *else_);

        ReturnInstruction *CreateRet();
        ReturnInstruction *CreateRet(Value *value);

        SelectInstruction *CreateSelect(
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
            Assert(!!m_InsertBlock, "insert block must not be null");

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

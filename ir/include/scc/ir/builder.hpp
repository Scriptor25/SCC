#pragma once

#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

#include <scc/assert.hpp>

#include <string_view>

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

        OperatorInstruction *CreateOperator(
            Operator operator_,
            std::vector<Value *> operands,
            std::string name = {});

        OperatorInstruction *CreateAdd(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateSub(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateMul(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateSDiv(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateUDiv(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateSRem(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateURem(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateAnd(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateOr(std::vector<Value *> operands, std::string name = {});
        OperatorInstruction *CreateXor(std::vector<Value *> operands, std::string name = {});

        ComparatorInstruction *CreateComparator(
            Comparator comparator,
            Value *lhs,
            Value *rhs,
            std::string name = {});

        ComparatorInstruction *CreateSLT(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateULT(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateSGT(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateUGT(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateSLE(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateULE(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateSGE(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateUGE(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateEQ(Value *lhs, Value *rhs, std::string name = {});
        ComparatorInstruction *CreateNE(Value *lhs, Value *rhs, std::string name = {});

        DirectBranchInstruction *CreateBranch(Block *destination);
        BranchInstruction *CreateBranch(
            Value *condition,
            Block *then,
            Block *else_);

        ReturnInstruction *CreateRet();
        ReturnInstruction *CreateRet(Value *value);

        SelectInstruction *CreateSelect(
            std::vector<std::pair<Block *, Value *>> options,
            std::string name = {});

        AllocInstruction *CreateAlloc(
            Type *type,
            std::string name = {},
            unsigned count = 1u);
        LoadInstruction *CreateLoad(
            Value *pointer,
            std::string name = {});
        StoreInstruction *CreateStore(
            Value *pointer,
            Value *value);

        OffsetInstruction *CreateOffset(
            Type *type,
            Value *base,
            std::vector<Value *> offsets,
            std::string name = {});

        OffsetInstruction *CreateConstOffset(
            Value *base,
            const std::vector<unsigned> &offsets,
            std::string name = {});

        CallInstruction *CreateCall(
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

            auto value = std::make_unique<T>(std::forward<Args>(args)...);
            auto *ptr = value.get();

            m_InsertBlock->Insert(std::move(value));

            return ptr;
        }

    private:
        Context &m_Context;
        Block *m_InsertBlock;
    };
}

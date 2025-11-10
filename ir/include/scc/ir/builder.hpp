#pragma once

#include <scc/assert.hpp>
#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

template<typename T>
concept InstructionLike = std::is_base_of_v<scc::ir::Instruction, T>;

namespace scc::ir
{
    class Builder final
    {
    public:
        explicit Builder(Context &context);

        [[nodiscard]] ConstantInt::Ptr CreateI8(uint8_t value) const;
        [[nodiscard]] ConstantInt::Ptr CreateI16(uint16_t value) const;
        [[nodiscard]] ConstantInt::Ptr CreateI32(uint32_t value) const;
        [[nodiscard]] ConstantInt::Ptr CreateI64(uint64_t value) const;

        [[nodiscard]] ConstantFloat::Ptr CreateF32(float value) const;
        [[nodiscard]] ConstantFloat::Ptr CreateF64(double value) const;

        [[nodiscard]] ConstantArray::Ptr CreateArray(std::vector<ConstantFwd::Ptr> values) const;
        [[nodiscard]] ConstantVector::Ptr CreateVector(std::vector<ConstantFwd::Ptr> values) const;
        [[nodiscard]] ConstantStruct::Ptr CreateStruct(std::vector<ConstantFwd::Ptr> values) const;

        [[nodiscard]] Variable::Ptr CreateString(
            Module &module,
            std::string name,
            const std::string &value) const;

        [[nodiscard]] BlockFwd::Ptr CreateBlock(
            const Function::Ptr &function,
            std::string name) const;

        void SetInsertBlock(BlockFwd::Ptr block);
        void ClearInsertBlock();
        [[nodiscard]] BlockFwd::Ptr GetInsertBlock() const;

        OperatorInstruction::Ptr CreateOperator(
            Operator operator_,
            std::vector<ValueFwd::Ptr> operands,
            std::string name = {});

        OperatorInstruction::Ptr CreateAdd(std::vector<ValueFwd::Ptr> operands, std::string name = {});
        OperatorInstruction::Ptr CreateSub(std::vector<ValueFwd::Ptr> operands, std::string name = {});
        OperatorInstruction::Ptr CreateMul(std::vector<ValueFwd::Ptr> operands, std::string name = {});
        OperatorInstruction::Ptr CreateDiv(std::vector<ValueFwd::Ptr> operands, std::string name = {});
        OperatorInstruction::Ptr CreateRem(std::vector<ValueFwd::Ptr> operands, std::string name = {});
        OperatorInstruction::Ptr CreateAnd(std::vector<ValueFwd::Ptr> operands, std::string name = {});
        OperatorInstruction::Ptr CreateOr(std::vector<ValueFwd::Ptr> operands, std::string name = {});
        OperatorInstruction::Ptr CreateXor(std::vector<ValueFwd::Ptr> operands, std::string name = {});

        ComparatorInstruction::Ptr CreateComparator(
            Comparator comparator,
            ValueFwd::Ptr lhs,
            ValueFwd::Ptr rhs,
            std::string name = {});

        ComparatorInstruction::Ptr CreateLT(ValueFwd::Ptr lhs, ValueFwd::Ptr rhs, std::string name = {});
        ComparatorInstruction::Ptr CreateGT(ValueFwd::Ptr lhs, ValueFwd::Ptr rhs, std::string name = {});
        ComparatorInstruction::Ptr CreateLE(ValueFwd::Ptr lhs, ValueFwd::Ptr rhs, std::string name = {});
        ComparatorInstruction::Ptr CreateGE(ValueFwd::Ptr lhs, ValueFwd::Ptr rhs, std::string name = {});
        ComparatorInstruction::Ptr CreateEQ(ValueFwd::Ptr lhs, ValueFwd::Ptr rhs, std::string name = {});
        ComparatorInstruction::Ptr CreateNE(ValueFwd::Ptr lhs, ValueFwd::Ptr rhs, std::string name = {});

        BranchInstruction::Ptr CreateBranch(BlockFwd::Ptr destination);
        BranchInstruction::Ptr CreateBranch(
            ValueFwd::Ptr condition,
            BlockFwd::Ptr then,
            BlockFwd::Ptr else_);

        ReturnInstruction::Ptr CreateRet();
        ReturnInstruction::Ptr CreateRet(ValueFwd::Ptr value);

        SelectInstruction::Ptr CreateSelect(
            std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> options,
            std::string name = {});

        AllocInstruction::Ptr CreateAlloc(
            TypeFwd::Ptr type,
            std::string name = {},
            unsigned count = 1u);
        LoadInstruction::Ptr CreateLoad(
            ValueFwd::Ptr pointer,
            std::string name = {});
        StoreInstruction::Ptr CreateStore(
            ValueFwd::Ptr pointer,
            ValueFwd::Ptr value);

        OffsetInstruction::Ptr CreateOffset(
            TypeFwd::Ptr type,
            ValueFwd::Ptr base,
            std::vector<ValueFwd::Ptr> offsets,
            std::string name = {});

        OffsetInstruction::Ptr CreateConstOffset(
            ValueFwd::Ptr base,
            const std::vector<unsigned> &offsets,
            std::string name = {});

        CallInstruction::Ptr CreateCall(
            ValueFwd::Ptr callee,
            std::vector<ValueFwd::Ptr> arguments,
            std::string name = {});

        CastInstruction::Ptr CreateCast(
            TypeFwd::Ptr type,
            ValueFwd::Ptr value,
            std::string name = {});

        template<InstructionLike T, typename... Args>
        T::Ptr Create(Args &&... args)
        {
            Assert(!!m_InsertBlock, "insert block must not be null");

            auto instruction = std::make_shared<T>(std::forward<Args>(args)...);
            m_InsertBlock->Insert(instruction);
            return instruction;
        }

    private:
        Context &m_Context;
        BlockFwd::Ptr m_InsertBlock;
    };
}

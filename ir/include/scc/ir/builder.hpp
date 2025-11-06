#pragma once

#include <scc/ir/instruction.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

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

        [[nodiscard]] ConstantArray::Ptr CreateArray(std::vector<ConstantPtr> values) const;
        [[nodiscard]] ConstantVector::Ptr CreateVector(std::vector<ConstantPtr> values) const;
        [[nodiscard]] ConstantStruct::Ptr CreateStruct(std::vector<ConstantPtr> values) const;

        [[nodiscard]] BlockPtr CreateBlock(const Function::Ptr &function, std::string name) const;

        void SetInsertBlock(BlockPtr block);
        [[nodiscard]] BlockPtr GetInsertBlock() const;

        OperatorInstruction::Ptr CreateOperator(
            TypePtr type,
            std::string name,
            Operator operator_,
            std::vector<ValuePtr> operands);
        ComparatorInstruction::Ptr CreateComparator(
            TypePtr type,
            std::string name,
            Comparator comparator,
            ValuePtr lhs,
            ValuePtr rhs);
        BranchInstruction::Ptr CreateBranch(BlockPtr destination);
        BranchInstruction::Ptr CreateBranch(ValuePtr condition, BlockPtr then, BlockPtr else_);

    private:
        Context &m_Context;
        BlockPtr m_InsertBlock;
    };
}

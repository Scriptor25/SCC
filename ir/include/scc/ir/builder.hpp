#pragma once

#include <scc/ir/instruction.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

template<typename T>
concept BlockValuePairLike = requires(T t)
{
    { std::get<0>(t) } -> std::convertible_to<scc::ir::BlockPtr>;
    { std::get<1>(t) } -> std::convertible_to<scc::ir::ValuePtr>;
};

template<typename T>
concept ValueLike = std::convertible_to<T, scc::ir::ValuePtr>;

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

        [[nodiscard]] Variable::Ptr CreateString(Module &module, std::string name, const std::string &value) const;

        [[nodiscard]] BlockPtr CreateBlock(const Function::Ptr &function, std::string name) const;

        void SetInsertBlock(BlockPtr block);
        void ClearInsertBlock();
        [[nodiscard]] BlockPtr GetInsertBlock() const;

        OperatorInstruction::Ptr CreateOperator(
            std::string name,
            Operator operator_,
            std::vector<ValuePtr> operands);

        template<ValueLike... Operands>
        OperatorInstruction::Ptr CreateOperator(
            std::string name,
            const Operator operator_,
            Operands &&... operands)
        {
            return CreateOperator(
                std::move(name),
                operator_,
                std::vector<ValuePtr>{ std::forward<Operands>(operands)... });
        }

        ComparatorInstruction::Ptr CreateComparator(
            std::string name,
            Comparator comparator,
            ValuePtr lhs,
            ValuePtr rhs);
        BranchInstruction::Ptr CreateBranch(BlockPtr destination);
        BranchInstruction::Ptr CreateBranch(ValuePtr condition, BlockPtr then, BlockPtr else_);
        RetInstruction::Ptr CreateRet();
        RetInstruction::Ptr CreateRet(ValuePtr value);

        SelectInstruction::Ptr CreateSelect(std::string name, std::vector<std::pair<BlockPtr, ValuePtr>> options);

        template<BlockValuePairLike... Options>
        SelectInstruction::Ptr CreateSelect(std::string name, Options &&... options)
        {
            return CreateSelect(
                std::move(name),
                std::vector<std::pair<BlockPtr, ValuePtr>>{ std::forward<Options>(options)... });
        }

        AllocInstruction::Ptr CreateAlloc(std::string name, TypePtr type);
        LoadInstruction::Ptr CreateLoad(std::string name, ValuePtr pointer);
        StoreInstruction::Ptr CreateStore(ValuePtr pointer, ValuePtr value);

        OffsetInstruction::Ptr CreateOffset(std::string name, ValuePtr base, std::vector<ValuePtr> offsets);

        template<ValueLike... Offsets>
        OffsetInstruction::Ptr CreateOffset(std::string name, ValuePtr base, Offsets &&... offsets)
        {
            return CreateOffset(
                std::move(name),
                std::move(base),
                std::vector<ValuePtr>{ std::forward<Offsets>(offsets)... });
        }

        CallInstruction::Ptr CreateCall(std::string name, ValuePtr callee, std::vector<ValuePtr> arguments);

        template<ValueLike... Arguments>
        CallInstruction::Ptr CreateCall(std::string name, ValuePtr callee, Arguments &&... arguments)
        {
            return CreateCall(
                std::move(name),
                std::move(callee),
                std::vector<ValuePtr>{ std::forward<Arguments>(arguments)... });
        }

        CallVoidInstruction::Ptr CreateCallVoid(ValuePtr callee, std::vector<ValuePtr> arguments);

        template<ValueLike... Arguments>
        CallVoidInstruction::Ptr CreateCallVoid(ValuePtr callee, Arguments &&... arguments)
        {
            return CreateCallVoid(
                std::move(callee),
                std::vector<ValuePtr>{ std::forward<Arguments>(arguments)... });
        }

        CastInstruction::Ptr CreateCast(std::string name, ValuePtr value, TypePtr type);

    private:
        Context &m_Context;
        BlockPtr m_InsertBlock;
    };
}

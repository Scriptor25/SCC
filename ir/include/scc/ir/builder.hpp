#pragma once

#include <scc/assert.hpp>
#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/value.hpp>

template<typename T>
concept BlockValuePairLike = requires(T t)
{
    { std::get<0>(t) } -> std::convertible_to<scc::ir::BlockFwd::Ptr>;
    { std::get<1>(t) } -> std::convertible_to<scc::ir::ValueFwd::Ptr>;
};

template<typename T>
concept ValueLike = std::convertible_to<T, scc::ir::ValueFwd::Ptr>;

template<typename T>
concept OffsetLike = std::convertible_to<T, unsigned>;

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
            std::string name,
            Operator operator_,
            std::vector<ValueFwd::Ptr> operands);

        template<ValueLike... Operands>
        OperatorInstruction::Ptr CreateOperator(
            std::string name,
            const Operator operator_,
            Operands &&... operands)
        {
            return CreateOperator(
                std::move(name),
                operator_,
                std::vector<ValueFwd::Ptr>{ std::forward<Operands>(operands)... });
        }

        ComparatorInstruction::Ptr CreateComparator(
            std::string name,
            Comparator comparator,
            ValueFwd::Ptr lhs,
            ValueFwd::Ptr rhs);

        BranchInstruction::Ptr CreateBranch(BlockFwd::Ptr destination);
        BranchInstruction::Ptr CreateBranch(
            ValueFwd::Ptr condition,
            BlockFwd::Ptr then,
            BlockFwd::Ptr else_);

        RetInstruction::Ptr CreateRet();
        RetInstruction::Ptr CreateRet(ValueFwd::Ptr value);

        SelectInstruction::Ptr CreateSelect(
            std::string name,
            std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>> options);

        template<BlockValuePairLike... Options>
        SelectInstruction::Ptr CreateSelect(
            std::string name,
            Options &&... options)
        {
            return CreateSelect(
                std::move(name),
                std::vector<std::pair<BlockFwd::Ptr, ValueFwd::Ptr>>{ std::forward<Options>(options)... });
        }

        AllocInstruction::Ptr CreateAlloc(
            std::string name,
            TypeFwd::Ptr type,
            unsigned count = 1u);
        LoadInstruction::Ptr CreateLoad(
            std::string name,
            ValueFwd::Ptr pointer);
        StoreInstruction::Ptr CreateStore(
            ValueFwd::Ptr pointer,
            ValueFwd::Ptr value);

        OffsetInstruction::Ptr CreateOffset(
            std::string name,
            TypeFwd::Ptr type,
            ValueFwd::Ptr base,
            std::vector<ValueFwd::Ptr> offsets);

        template<ValueLike... Offsets>
        OffsetInstruction::Ptr CreateOffset(
            std::string name,
            TypeFwd::Ptr type,
            ValueFwd::Ptr base,
            Offsets &&... offsets)
        {
            return CreateOffset(
                std::move(name),
                std::move(type),
                std::move(base),
                std::vector<ValueFwd::Ptr>{ std::forward<Offsets>(offsets)... });
        }

        OffsetInstruction::Ptr CreateConstOffset(
            std::string name,
            ValueFwd::Ptr base,
            const std::vector<unsigned> &offsets);

        template<OffsetLike... Offsets>
        OffsetInstruction::Ptr CreateConstOffset(
            std::string name,
            ValueFwd::Ptr base,
            Offsets &&... offsets)
        {
            return CreateConstOffset(
                std::move(name),
                std::move(base),
                std::vector<unsigned>{ std::forward<Offsets>(offsets)... });
        }

        CallInstruction::Ptr CreateCall(
            std::string name,
            ValueFwd::Ptr callee,
            std::vector<ValueFwd::Ptr> arguments);

        template<ValueLike... Arguments>
        CallInstruction::Ptr CreateCall(
            std::string name,
            ValueFwd::Ptr callee,
            Arguments &&... arguments)
        {
            return CreateCall(
                std::move(name),
                std::move(callee),
                std::vector<ValueFwd::Ptr>{ std::forward<Arguments>(arguments)... });
        }

        CallVoidInstruction::Ptr CreateCallVoid(
            ValueFwd::Ptr callee,
            std::vector<ValueFwd::Ptr> arguments);

        template<ValueLike... Arguments>
        CallVoidInstruction::Ptr CreateCallVoid(
            ValueFwd::Ptr callee,
            Arguments &&... arguments)
        {
            return CreateCallVoid(
                std::move(callee),
                std::vector<ValueFwd::Ptr>{ std::forward<Arguments>(arguments)... });
        }

        CastInstruction::Ptr CreateCast(
            std::string name,
            ValueFwd::Ptr value,
            TypeFwd::Ptr type);

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

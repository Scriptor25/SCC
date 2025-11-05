#pragma once

#include <vector>
#include <scc/ir/ir.hpp>
#include <scc/ir/type.hpp>

namespace scc::ir
{
    class Value
    {
    public:
        explicit Value(TypePtr type);
        virtual ~Value() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

        [[nodiscard]] TypePtr GetType() const;

    protected:
        TypePtr m_Type;
    };

    class Global : public Value
    {
    public:
        explicit Global(TypePtr type, std::string name);

    protected:
        std::string m_Name;
    };

    class Variable final : public Global, public Shared<Variable>
    {
    public:
        explicit Variable(TypePtr type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ConstantPtr m_Initializer;
    };

    class Function final : public Global, public Shared<Function>
    {
    public:
        explicit Function(TypePtr type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;

        void Append(BlockPtr block);

    private:
        std::vector<BlockPtr> m_Blocks;
    };

    class Constant : public Value
    {
    public:
        explicit Constant(TypePtr type);
    };

    class ConstantInt final : public Constant, public Shared<ConstantInt>
    {
    public:
        explicit ConstantInt(IntType::Ptr type, uint64_t value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        uint64_t m_Value;
    };

    class ConstantFloat final : public Constant, public Shared<ConstantFloat>
    {
    public:
        explicit ConstantFloat(FloatType::Ptr type, double value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        double m_Value;
    };

    class ConstantArray final : public Constant, public Shared<ConstantArray>
    {
    public:
        explicit ConstantArray(ArrayType::Ptr type, std::vector<ConstantPtr> values);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<ConstantPtr> m_Values;
    };

    class ConstantVector final : public Constant, public Shared<ConstantVector>
    {
    public:
        explicit ConstantVector(VectorType::Ptr type, std::vector<ConstantPtr> values);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<ConstantPtr> m_Values;
    };

    class ConstantStruct final : public Constant, public Shared<ConstantStruct>
    {
    public:
        explicit ConstantStruct(StructType::Ptr type, std::vector<ConstantPtr> values);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<ConstantPtr> m_Values;
    };
}

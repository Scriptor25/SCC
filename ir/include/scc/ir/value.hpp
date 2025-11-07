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
        virtual std::ostream &PrintOperand(std::ostream &stream) const = 0;

        [[nodiscard]] TypePtr GetType() const;

    protected:
        TypePtr m_Type;
    };

    class NamedValue : public Value
    {
    public:
        explicit NamedValue(TypePtr type, std::string name);

        [[nodiscard]] std::string GetName() const;
        void SetName(std::string name);

    protected:
        std::string m_Name;
    };

    class Argument final : public NamedValue, public Shared<Argument>
    {
    public:
        explicit Argument(TypePtr type, std::string name);

        std::ostream &PrintOperand(std::ostream &stream) const override;
        std::ostream &Print(std::ostream &stream) const override;
    };

    class Global : public NamedValue
    {
    public:
        explicit Global(TypePtr type, std::string name);

        std::ostream &PrintOperand(std::ostream &stream) const override;
    };

    class Variable final : public Global, public Shared<Variable>
    {
    public:
        explicit Variable(TypePtr type, std::string name, ConstantPtr initializer);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ConstantPtr m_Initializer;
    };

    class Function final : public Global, public Shared<Function>
    {
    public:
        explicit Function(const FunctionType::Ptr &type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;

        void Insert(BlockPtr block);

        unsigned GetArgumentCount() const;
        Argument::Ptr GetArgument(unsigned index) const;

    private:
        TypePtr m_Result;
        std::vector<Argument::Ptr> m_Arguments;
        bool m_Variadic;
        std::vector<BlockPtr> m_Blocks;
    };

    class Constant : public Value
    {
    public:
        explicit Constant(TypePtr type);

        std::ostream &Print(std::ostream &stream) const override;
    };

    class ConstantInt final : public Constant, public Shared<ConstantInt>
    {
    public:
        explicit ConstantInt(IntType::Ptr type, uint64_t value);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        uint64_t GetValue() const;

    private:
        uint64_t m_Value;
    };

    class ConstantFloat final : public Constant, public Shared<ConstantFloat>
    {
    public:
        explicit ConstantFloat(FloatType::Ptr type, double value);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        double GetValue() const;

    private:
        double m_Value;
    };

    class ConstantArray final : public Constant, public Shared<ConstantArray>
    {
    public:
        explicit ConstantArray(ArrayType::Ptr type, std::vector<ConstantPtr> values);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        unsigned GetValueCount() const;
        ConstantPtr GetValue(unsigned index) const;

    private:
        std::vector<ConstantPtr> m_Values;
    };

    class ConstantVector final : public Constant, public Shared<ConstantVector>
    {
    public:
        explicit ConstantVector(VectorType::Ptr type, std::vector<ConstantPtr> values);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        unsigned GetValueCount() const;
        ConstantPtr GetValue(unsigned index) const;

    private:
        std::vector<ConstantPtr> m_Values;
    };

    class ConstantStruct final : public Constant, public Shared<ConstantStruct>
    {
    public:
        explicit ConstantStruct(StructType::Ptr type, std::vector<ConstantPtr> values);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        unsigned GetValueCount() const;
        ConstantPtr GetValue(unsigned index) const;

    private:
        std::vector<ConstantPtr> m_Values;
    };
}

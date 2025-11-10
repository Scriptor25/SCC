#pragma once

#include <vector>
#include <scc/ir/ir.hpp>
#include <scc/ir/type.hpp>
#include <scc/ir/usable.hpp>

namespace scc::ir
{
    class Value : public Usable
    {
    public:
        explicit Value(TypeFwd::Ptr type);

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual std::ostream &PrintOperand(std::ostream &stream) const = 0;

        [[nodiscard]] TypeFwd::Ptr GetType() const;

    protected:
        TypeFwd::Ptr m_Type;
    };

    class IdentifiedValue : public Value
    {
    public:
        explicit IdentifiedValue(TypeFwd::Ptr type, RegisterFwd::Ptr register_);

        [[nodiscard]] RegisterFwd::Ptr GetRegister() const;
        void SetRegister(RegisterFwd::Ptr register_);

        std::ostream &Print(std::ostream &stream) const override;
        std::ostream &PrintOperand(std::ostream &stream) const override;

    protected:
        RegisterFwd::Ptr m_Register;
    };

    class Argument final : public IdentifiedValue, public Shared<Argument>
    {
    public:
        explicit Argument(TypeFwd::Ptr type, RegisterFwd::Ptr register_);

        std::ostream &Print(std::ostream &stream) const override;

        void SetName(std::string name) const;
    };

    class Global : public Value
    {
    public:
        explicit Global(TypeFwd::Ptr type, std::string name);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        std::string GetName() const;
        void SetName(std::string name);

    protected:
        std::string m_Name;
    };

    class Variable final : public Global, public Shared<Variable>
    {
    public:
        explicit Variable(TypeFwd::Ptr type, std::string name, ConstantFwd::Ptr initializer);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ConstantFwd::Ptr m_Initializer;
    };

    class Function final : public Global, public Shared<Function>
    {
    public:
        explicit Function(const FunctionType::Ptr &type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;

        void Insert(BlockFwd::Ptr block);

        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] Argument::Ptr GetArgument(unsigned index) const;

        RegisterFwd::Ptr CreateRegister(std::string name = {});

    private:
        TypeFwd::Ptr m_Result;
        std::vector<Argument::Ptr> m_Arguments;
        bool m_Variadic;

        std::vector<BlockFwd::Ptr> m_Blocks;
        std::vector<RegisterFwd::Ptr> m_Registers;
    };

    class Constant : public Value
    {
    public:
        explicit Constant(TypeFwd::Ptr type);

        std::ostream &Print(std::ostream &stream) const override;
    };

    class ConstantInt final : public Constant, public Shared<ConstantInt>
    {
    public:
        explicit ConstantInt(IntType::Ptr type, uint64_t value);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] uint64_t GetValue() const;

    private:
        uint64_t m_Value;
    };

    class ConstantFloat final : public Constant, public Shared<ConstantFloat>
    {
    public:
        explicit ConstantFloat(FloatType::Ptr type, double value);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] double GetValue() const;

    private:
        double m_Value;
    };

    class ConstantArray final : public Constant, public Shared<ConstantArray>
    {
    public:
        explicit ConstantArray(ArrayType::Ptr type, std::vector<ConstantFwd::Ptr> values);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetValueCount() const;
        [[nodiscard]] ConstantFwd::Ptr GetValue(unsigned index) const;

    private:
        std::vector<ConstantFwd::Ptr> m_Values;
    };

    class ConstantVector final : public Constant, public Shared<ConstantVector>
    {
    public:
        explicit ConstantVector(VectorType::Ptr type, std::vector<ConstantFwd::Ptr> values);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetValueCount() const;
        [[nodiscard]] ConstantFwd::Ptr GetValue(unsigned index) const;

    private:
        std::vector<ConstantFwd::Ptr> m_Values;
    };

    class ConstantStruct final : public Constant, public Shared<ConstantStruct>
    {
    public:
        explicit ConstantStruct(StructType::Ptr type, std::vector<ConstantFwd::Ptr> values);

        std::ostream &PrintOperand(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetValueCount() const;
        [[nodiscard]] ConstantFwd::Ptr GetValue(unsigned index) const;

    private:
        std::vector<ConstantFwd::Ptr> m_Values;
    };
}

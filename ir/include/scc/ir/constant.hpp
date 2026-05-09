#pragma once

#include <scc/ir/ir.hpp>
#include <scc/ir/user.hpp>

#include <cstdint>
#include <iosfwd>
#include <vector>

namespace scc::ir
{
    class Constant : public User
    {
    public:
        explicit Constant(Type *type);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] virtual bool Compare(Constant *value) const = 0;
    };

    class ConstantInt final : public Constant
    {
    public:
        explicit ConstantInt(IntType *type, uint64_t value);

        [[nodiscard]] std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;
        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] uint64_t GetValue() const;

    private:
        uint64_t m_Value;
    };

    class ConstantFloat final : public Constant
    {
    public:
        explicit ConstantFloat(FloatType *type, float64_t value);

        [[nodiscard]] std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;
        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] float64_t GetValue() const;

    private:
        float64_t m_Value;
    };

    class ConstantArray final : public Constant
    {
    public:
        explicit ConstantArray(ArrayType *type, std::vector<Constant *> values);
        ~ConstantArray() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        [[nodiscard]] std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;
        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] unsigned GetValueCount() const;
        [[nodiscard]] Constant *GetValue(unsigned index) const;

    private:
        std::vector<Constant *> m_Values;
    };

    class ConstantVector final : public Constant
    {
    public:
        explicit ConstantVector(VectorType *type, std::vector<Constant *> values);
        ~ConstantVector() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        [[nodiscard]] std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;
        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] unsigned GetValueCount() const;
        [[nodiscard]] Constant *GetValue(unsigned index) const;

    private:
        std::vector<Constant *> m_Values;
    };

    class ConstantStruct final : public Constant
    {
    public:
        explicit ConstantStruct(StructType *type, std::vector<Constant *> values);
        ~ConstantStruct() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        [[nodiscard]] std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;
        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] unsigned GetValueCount() const;
        [[nodiscard]] Constant *GetValue(unsigned index) const;

    private:
        std::vector<Constant *> m_Values;
    };
}

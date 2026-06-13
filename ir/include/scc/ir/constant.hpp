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

    class ConstantInt : public Constant
    {
    public:
        explicit ConstantInt(IntType *type, uint64_t value);

        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;
        std::ostream &PrintOperandAssembly(std::ostream &stream, LoweringContext &context, bool address) const override;

        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] uint64_t GetValue() const;

    private:
        uint64_t m_Value;
    };

    class ConstantFloat : public Constant
    {
    public:
        explicit ConstantFloat(FloatType *type, float64_t value);

        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;
        std::ostream &PrintOperandAssembly(std::ostream &stream, LoweringContext &context, bool address) const override;

        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] float64_t GetValue() const;

    private:
        float64_t m_Value;
    };

    class ConstantArray : public Constant
    {
    public:
        explicit ConstantArray(ArrayType *type, std::vector<Constant *> elements);
        ~ConstantArray() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;
        std::ostream &PrintOperandAssembly(std::ostream &stream, LoweringContext &context, bool address) const override;

        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] size_t GetElementCount() const;
        [[nodiscard]] Constant *GetElement(size_t index) const;

    private:
        std::vector<Constant *> m_Elements;
    };

    class ConstantVector : public Constant
    {
    public:
        explicit ConstantVector(VectorType *type, std::vector<Constant *> elements);
        ~ConstantVector() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;
        std::ostream &PrintOperandAssembly(std::ostream &stream, LoweringContext &context, bool address) const override;

        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] size_t GetElementCount() const;
        [[nodiscard]] Constant *GetElement(size_t index) const;

    private:
        std::vector<Constant *> m_Elements;
    };

    class ConstantStruct : public Constant
    {
    public:
        explicit ConstantStruct(StructType *type, std::vector<Constant *> elements);
        ~ConstantStruct() override;

        void DropAll() override;
        void Replace(Value *value, Value *with) override;

        std::ostream &PrintOperand(std::ostream &stream, bool print_type) const override;

        std::ostream &PrintAssembly(std::ostream &stream, LoweringContext &context) const override;
        std::ostream &PrintOperandAssembly(std::ostream &stream, LoweringContext &context, bool address) const override;

        [[nodiscard]] bool Compare(Constant *value) const override;

        [[nodiscard]] size_t GetElementCount() const;
        [[nodiscard]] Constant *GetElement(size_t index) const;

    private:
        std::vector<Constant *> m_Elements;
    };
}

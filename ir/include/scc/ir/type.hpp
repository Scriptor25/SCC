#pragma once

#include <vector>
#include <scc/ir/ir.hpp>

namespace scc::ir
{
    enum Kind
    {
        Kind_Void,
        Kind_Int,
        Kind_Float,
        Kind_Pointer,
        Kind_Array,
        Kind_Vector,
        Kind_Struct,
        Kind_Function,
    };

    class Type
    {
    public:
        explicit Type(Context &context, Kind kind);
        virtual ~Type() = default;

        [[nodiscard]] virtual unsigned GenerateHash() const = 0;
        [[nodiscard]] virtual bool Equals(const TypePtr &) const = 0;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

        [[nodiscard]] Context &GetContext() const;
        [[nodiscard]] Kind GetKind() const;

    protected:
        Context &m_Context;
        Kind m_Kind;
    };

    class VoidType final : public Type, public Shared<VoidType>
    {
    public:
        explicit VoidType(Context &context);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;
    };

    class IntType final : public Type, public Shared<IntType>
    {
    public:
        explicit IntType(Context &context, unsigned size_bytes);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetSizeBytes() const;

    private:
        unsigned m_SizeBytes;
    };

    class FloatType final : public Type, public Shared<FloatType>
    {
    public:
        explicit FloatType(Context &context, unsigned size_bytes);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetSizeBytes() const;

    private:
        unsigned m_SizeBytes;
    };

    class PointerType final : public Type, public Shared<PointerType>
    {
    public:
        explicit PointerType(Context &context, TypePtr base);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypePtr GetBase() const;

    private:
        TypePtr m_Base;
    };

    class ArrayType final : public Type, public Shared<ArrayType>
    {
    public:
        explicit ArrayType(Context &context, TypePtr base, unsigned length);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypePtr GetBase() const;
        [[nodiscard]] unsigned GetLength() const;

    private:
        TypePtr m_Base;
        unsigned m_Length;
    };

    class VectorType final : public Type, public Shared<VectorType>
    {
    public:
        explicit VectorType(Context &context, TypePtr base, unsigned length);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypePtr GetBase() const;
        [[nodiscard]] unsigned GetLength() const;

    private:
        TypePtr m_Base;
        unsigned m_Length;
    };

    class StructType final : public Type, public Shared<StructType>
    {
    public:
        explicit StructType(Context &context, std::vector<TypePtr> elements);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetElementCount() const;
        [[nodiscard]] TypePtr GetElement(unsigned index) const;

        [[nodiscard]] std::vector<TypePtr>::const_iterator begin() const;
        [[nodiscard]] std::vector<TypePtr>::const_iterator end() const;

    private:
        std::vector<TypePtr> m_Elements;
    };

    class FunctionType final : public Type, public Shared<FunctionType>
    {
    public:
        explicit FunctionType(Context &context, TypePtr result, std::vector<TypePtr> arguments, bool variadic);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypePtr GetResult() const;

        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] TypePtr GetArgument(unsigned index) const;

        [[nodiscard]] std::vector<TypePtr>::const_iterator begin() const;
        [[nodiscard]] std::vector<TypePtr>::const_iterator end() const;

        [[nodiscard]] bool IsVariadic() const;

    private:
        TypePtr m_Result;
        std::vector<TypePtr> m_Arguments;
        bool m_Variadic;
    };
}

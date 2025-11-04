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
    };

    class Type
    {
    public:
        using Ptr = std::shared_ptr<Type>;

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

    class VoidType final : public Type
    {
    public:
        using Ptr = std::shared_ptr<VoidType>;

        explicit VoidType(Context &context);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;
    };

    class IntType final : public Type
    {
    public:
        using Ptr = std::shared_ptr<IntType>;

        explicit IntType(Context &context, unsigned size_bytes);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetSizeBytes() const;

    private:
        unsigned m_SizeBytes;
    };

    class FloatType final : public Type
    {
    public:
        using Ptr = std::shared_ptr<FloatType>;

        explicit FloatType(Context &context, unsigned size_bytes);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetSizeBytes() const;

    private:
        unsigned m_SizeBytes;
    };

    class PointerType final : public Type
    {
    public:
        using Ptr = std::shared_ptr<PointerType>;

        explicit PointerType(Context &context, TypePtr base);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypePtr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypePtr GetBase() const;

    private:
        TypePtr m_Base;
    };

    class ArrayType final : public Type
    {
    public:
        using Ptr = std::shared_ptr<ArrayType>;

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

    class VectorType final : public Type
    {
    public:
        using Ptr = std::shared_ptr<VectorType>;

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

    class StructType final : public Type
    {
    public:
        using Ptr = std::shared_ptr<StructType>;

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
}

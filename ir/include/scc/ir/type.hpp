#pragma once

#include <vector>
#include <scc/ir/ir.hpp>

namespace scc::ir
{
    enum class Kind
    {
        Void,
        Int,
        Float,
        Pointer,
        Array,
        Vector,
        Struct,
        Function,
    };

    class Type
    {
    public:
        explicit Type(Context &context, Kind kind);
        virtual ~Type() = default;

        [[nodiscard]] virtual unsigned GenerateHash() const = 0;
        [[nodiscard]] virtual bool Equals(const TypeFwd::Ptr &) const = 0;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

        [[nodiscard]] Context &GetContext() const;
        [[nodiscard]] Kind GetKind() const;

        [[nodiscard]] virtual unsigned GetElementCount() const;
        [[nodiscard]] virtual TypeFwd::Ptr GetElement(unsigned index) const;

    protected:
        Context &m_Context;
        Kind m_Kind;
    };

    class VoidType final : public Type, public Shared<VoidType>
    {
    public:
        explicit VoidType(Context &context);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;
    };

    class IntType final : public Type, public Shared<IntType>
    {
    public:
        explicit IntType(Context &context, unsigned size_bits);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetSizeBits() const;

    private:
        unsigned m_SizeBits;
    };

    class FloatType final : public Type, public Shared<FloatType>
    {
    public:
        explicit FloatType(Context &context, unsigned size_bits);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetSizeBits() const;

    private:
        unsigned m_SizeBits;
    };

    class PointerType final : public Type, public Shared<PointerType>
    {
    public:
        explicit PointerType(Context &context, TypeFwd::Ptr element);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypeFwd::Ptr GetElement() const;

        [[nodiscard]] unsigned GetElementCount() const override;
        [[nodiscard]] TypeFwd::Ptr GetElement(unsigned index) const override;

    private:
        TypeFwd::Ptr m_Element;
    };

    class ArrayType final : public Type, public Shared<ArrayType>
    {
    public:
        explicit ArrayType(Context &context, TypeFwd::Ptr element, unsigned length);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypeFwd::Ptr GetElement() const;
        [[nodiscard]] unsigned GetLength() const;

        [[nodiscard]] unsigned GetElementCount() const override;
        [[nodiscard]] TypeFwd::Ptr GetElement(unsigned index) const override;

    private:
        TypeFwd::Ptr m_Element;
        unsigned m_Length;
    };

    class VectorType final : public Type, public Shared<VectorType>
    {
    public:
        explicit VectorType(Context &context, TypeFwd::Ptr element, unsigned length);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypeFwd::Ptr GetElement() const;
        [[nodiscard]] unsigned GetLength() const;

        [[nodiscard]] unsigned GetElementCount() const override;
        [[nodiscard]] TypeFwd::Ptr GetElement(unsigned index) const override;

    private:
        TypeFwd::Ptr m_Element;
        unsigned m_Length;
    };

    class StructType final : public Type, public Shared<StructType>
    {
    public:
        explicit StructType(Context &context, std::vector<TypeFwd::Ptr> elements);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetElementCount() const override;
        [[nodiscard]] TypeFwd::Ptr GetElement(unsigned index) const override;

        [[nodiscard]] std::vector<TypeFwd::Ptr>::const_iterator begin() const;
        [[nodiscard]] std::vector<TypeFwd::Ptr>::const_iterator end() const;

    private:
        std::vector<TypeFwd::Ptr> m_Elements;
    };

    class FunctionType final : public Type, public Shared<FunctionType>
    {
    public:
        explicit FunctionType(
            Context &context,
            TypeFwd::Ptr result,
            std::vector<TypeFwd::Ptr> arguments,
            bool variadic);

        [[nodiscard]] unsigned GenerateHash() const override;
        [[nodiscard]] bool Equals(const TypeFwd::Ptr &type) const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] TypeFwd::Ptr GetResult() const;

        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] TypeFwd::Ptr GetArgument(unsigned index) const;

        [[nodiscard]] std::vector<TypeFwd::Ptr>::const_iterator begin() const;
        [[nodiscard]] std::vector<TypeFwd::Ptr>::const_iterator end() const;

        [[nodiscard]] bool IsVariadic() const;

    private:
        TypeFwd::Ptr m_Result;
        std::vector<TypeFwd::Ptr> m_Arguments;
        bool m_Variadic;
    };
}

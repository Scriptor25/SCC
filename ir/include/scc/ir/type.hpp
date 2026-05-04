#pragma once

#include <scc/ir/ir.hpp>

#include <format>
#include <sstream>
#include <vector>

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

        [[nodiscard]] virtual bool Compare(Type *) const = 0;
        [[nodiscard]] virtual size_t GetSize() const = 0;
        [[nodiscard]] virtual size_t GetAlign() const = 0;
        [[nodiscard]] virtual bool IsElement() const = 0;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

        [[nodiscard]] Context &GetContext() const;
        [[nodiscard]] Kind GetKind() const;

        [[nodiscard]] virtual size_t GetElementCount() const;
        [[nodiscard]] virtual Type *GetElement(size_t index) const;

    protected:
        Context &m_Context;
        Kind m_Kind;
    };

    class VoidType final : public Type
    {
    public:
        explicit VoidType(Context &context);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;
    };

    class IntType final : public Type
    {
    public:
        explicit IntType(Context &context, unsigned bit_width);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetBitWidth() const;

    private:
        unsigned m_BitWidth;
    };

    class FloatType final : public Type
    {
    public:
        explicit FloatType(Context &context, unsigned bit_width);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] unsigned GetBitWidth() const;

    private:
        unsigned m_BitWidth;
    };

    class PointerType final : public Type
    {
    public:
        explicit PointerType(Context &context, Type *element);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Type *GetElement() const;

        [[nodiscard]] size_t GetElementCount() const override;
        [[nodiscard]] Type *GetElement(size_t index) const override;

    private:
        Type *m_Element;
    };

    class ArrayType final : public Type
    {
    public:
        explicit ArrayType(Context &context, Type *element, unsigned length);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Type *GetElement() const;
        [[nodiscard]] unsigned GetLength() const;

        [[nodiscard]] size_t GetElementCount() const override;
        [[nodiscard]] Type *GetElement(size_t index) const override;

    private:
        Type *m_Element;
        unsigned m_Length;
    };

    class VectorType final : public Type
    {
    public:
        explicit VectorType(Context &context, Type *element, unsigned length);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Type *GetElement() const;
        [[nodiscard]] unsigned GetLength() const;

        [[nodiscard]] size_t GetElementCount() const override;
        [[nodiscard]] Type *GetElement(size_t index) const override;

    private:
        Type *m_Element;
        unsigned m_Length;
    };

    class StructType final : public Type
    {
    public:
        explicit StructType(Context &context, std::vector<Type *> elements);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] size_t GetElementCount() const override;
        [[nodiscard]] Type *GetElement(size_t index) const override;

        [[nodiscard]] std::vector<Type *>::const_iterator begin() const;
        [[nodiscard]] std::vector<Type *>::const_iterator end() const;

    private:
        std::vector<Type *> m_Elements;
    };

    class FunctionType final : public Type
    {
    public:
        explicit FunctionType(
            Context &context,
            Type *result,
            std::vector<Type *> arguments,
            bool variadic);

        [[nodiscard]] bool Compare(Type *type) const override;
        [[nodiscard]] size_t GetSize() const override;
        [[nodiscard]] size_t GetAlign() const override;
        [[nodiscard]] bool IsElement() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Type *GetResult() const;

        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] Type *GetArgument(unsigned index) const;

        [[nodiscard]] std::vector<Type *>::const_iterator begin() const;
        [[nodiscard]] std::vector<Type *>::const_iterator end() const;

        [[nodiscard]] bool IsVariadic() const;

    private:
        Type *m_Result;
        std::vector<Type *> m_Arguments;
        bool m_Variadic;
    };
}

template<std::derived_from<scc::ir::Type> T>
struct std::formatter<T *>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    static auto format(T *value, C &&ctx)
    {
        std::ostringstream stream;
        value->Print(stream);

        for (auto c : stream.view())
            *ctx.out()++ = c;

        return ctx.out();
    }
};

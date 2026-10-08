#pragma once

#include <scc/cc/cc.hpp>

#include <scc/ir/type.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace scc::cc
{
    struct Type
    {
        static Type *Collapse(Context &context, Type *left, Type *right);

        virtual ~Type() = default;

        [[nodiscard]] virtual size_t GetBitWidth(Context &context) const = 0;
        [[nodiscard]] virtual ir::Type *Generate(Builder &builder) const = 0;
    };

    struct VoidType : Type
    {
        explicit VoidType() = default;

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::VoidType *Generate(Builder &builder) const override;
    };

    enum class IntegerKind
    {
        Bool,
        Char,
        SignedChar,
        UnsignedChar,
        SignedShort,
        UnsignedShort,
        SignedInt,
        UnsignedInt,
        SignedLong,
        UnsignedLong,
        SignedLongLong,
        UnsignedLongLong,
    };

    struct IntegerType : Type
    {
        explicit IntegerType(IntegerKind kind);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::IntType *Generate(Builder &builder) const override;

        [[nodiscard]] bool IsSigned() const;
        [[nodiscard]] size_t GetBitWidth() const;
        [[nodiscard]] size_t GetRank() const;
        [[nodiscard]] IntegerKind GetSwap() const;

        IntegerKind Kind;
    };

    enum class FloatingPointKind
    {
        Float,
        Double,
        LongDouble,
    };

    struct FloatingPointType : Type
    {
        explicit FloatingPointType(FloatingPointKind kind);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::FloatType *Generate(Builder &builder) const override;

        FloatingPointKind Kind;
    };

    struct StructElement
    {
        Type *Ty{};
        std::optional<std::string> Name;
        std::optional<uint8_t> Bits;
    };

    struct StructType : Type
    {
        explicit StructType() = default;
        explicit StructType(std::string name);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::StructType *Generate(Builder &builder) const override;

        std::optional<std::string> Name;
        std::vector<StructElement> Elements;
    };

    struct UnionElement
    {
        Type *Ty{};
        std::optional<std::string> Name;
    };

    struct UnionType : Type
    {
        explicit UnionType() = default;
        explicit UnionType(std::string name);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::ArrayType *Generate(Builder &builder) const override;

        std::optional<std::string> Name;
        std::vector<UnionElement> Elements;
    };

    struct EnumElement
    {
        std::string Name;
        std::optional<int64_t> Value;
    };

    struct EnumType : Type
    {
        explicit EnumType() = default;
        explicit EnumType(std::string name);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::IntType *Generate(Builder &builder) const override;

        [[nodiscard]] IntegerType *DetermineType(Context &context) const;

        std::optional<std::string> Name;
        std::vector<EnumElement> Elements;
        IntegerType *TypeOverride{};
    };

    struct PointerType : Type
    {
        explicit PointerType() = default;
        explicit PointerType(Type *element);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::PointerType *Generate(Builder &builder) const override;

        Type *Element{};
    };

    struct ArrayType : Type
    {
        explicit ArrayType(Type *element, size_t count);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::ArrayType *Generate(Builder &builder) const override;

        Type *Element{};
        size_t Count;
    };

    struct FunctionType : Type
    {
        explicit FunctionType(Type *result, std::vector<Type *> arguments, bool variadic);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::FunctionType *Generate(Builder &builder) const override;

        Type *Result;
        std::vector<Type *> Arguments;
        bool Variadic;
    };
}

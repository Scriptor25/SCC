#pragma once

#include <scc/cc/cc.hpp>

#include <scc/ir/type.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace scc::cc
{
    enum class TypeKind
    {
        Void,
        Integer,
        FloatingPoint,
        Struct,
        Union,
        Enum,
        Pointer,
        Array,
        Function,
    };

    struct Type
    {
        static const Type *Collapse(Context &context, const Type *a, const Type *b);

        explicit Type(TypeKind kind);
        virtual ~Type() = default;

        [[nodiscard]] bool IsVoid() const;
        [[nodiscard]] bool IsInteger() const;
        [[nodiscard]] bool IsFloatingPoint() const;
        [[nodiscard]] bool IsStruct() const;
        [[nodiscard]] bool IsUnion() const;
        [[nodiscard]] bool IsEnum() const;
        [[nodiscard]] bool IsPointer() const;
        [[nodiscard]] bool IsArray() const;
        [[nodiscard]] bool IsFunction() const;

        [[nodiscard]] virtual size_t GetBitWidth(Context &context) const = 0;
        [[nodiscard]] virtual ir::Type *Generate(Builder &builder) const = 0;

        [[nodiscard]] virtual const Type *Decay(Context &context) const = 0;

        TypeKind Kind;
    };

    struct VoidType : Type
    {
        explicit VoidType();

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::VoidType *Generate(Builder &builder) const override;

        [[nodiscard]] const Type *Decay(Context &context) const override;
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

        [[nodiscard]] const Type *Decay(Context &context) const override;

        [[nodiscard]] bool IsSigned() const;
        [[nodiscard]] size_t GetBitWidth() const;
        [[nodiscard]] size_t GetRank() const;
        [[nodiscard]] IntegerKind GetSwap() const;

        IntegerKind IKind;
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

        [[nodiscard]] const Type *Decay(Context &context) const override;

        [[nodiscard]] size_t GetBitWidth() const;
        [[nodiscard]] size_t GetRank() const;

        FloatingPointKind FKind;
    };

    struct StructElement
    {
        bool operator==(const StructElement &other) const;

        QualifiedType Ty;
        std::optional<std::string> Name;
        std::optional<uint8_t> Bits;
    };

    struct StructType : Type
    {
        explicit StructType(std::string name);
        explicit StructType(std::vector<StructElement> elements);
        explicit StructType(std::string name, std::vector<StructElement> elements);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::StructType *Generate(Builder &builder) const override;

        [[nodiscard]] const Type *Decay(Context &context) const override;

        std::optional<std::string> Name;
        std::vector<StructElement> Elements;
    };

    struct UnionElement
    {
        bool operator==(const UnionElement &other) const;

        QualifiedType Ty;
        std::optional<std::string> Name;
    };

    struct UnionType : Type
    {
        explicit UnionType(std::string name);
        explicit UnionType(std::vector<UnionElement> elements);
        explicit UnionType(std::string name, std::vector<UnionElement> elements);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::ArrayType *Generate(Builder &builder) const override;

        [[nodiscard]] const Type *Decay(Context &context) const override;

        std::optional<std::string> Name;
        std::vector<UnionElement> Elements;
    };

    struct EnumElement
    {
        bool operator==(const EnumElement &other) const;

        std::string Name;
        std::optional<int64_t> Value;
    };

    struct EnumType : Type
    {
        explicit EnumType(std::string name);
        explicit EnumType(std::vector<EnumElement> elements);
        explicit EnumType(std::string name, std::vector<EnumElement> elements);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::IntType *Generate(Builder &builder) const override;

        [[nodiscard]] const Type *Decay(Context &context) const override;

        [[nodiscard]] const IntegerType *DetermineType(Context &context) const;

        std::optional<std::string> Name;
        std::vector<EnumElement> Elements;
        const IntegerType *TypeOverride{};
    };

    struct PointerType : Type
    {
        explicit PointerType(QualifiedType element);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::PointerType *Generate(Builder &builder) const override;

        [[nodiscard]] const Type *Decay(Context &context) const override;

        QualifiedType Element;
    };

    struct ArrayType : Type
    {
        explicit ArrayType(
            QualifiedType element,
            size_t count);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::ArrayType *Generate(Builder &builder) const override;

        [[nodiscard]] const Type *Decay(Context &context) const override;

        QualifiedType Element;
        size_t Count;
    };

    struct FunctionType : Type
    {
        explicit FunctionType(
            QualifiedType result,
            std::vector<QualifiedType> arguments,
            bool variadic);

        [[nodiscard]] size_t GetBitWidth(Context &context) const override;
        [[nodiscard]] ir::FunctionType *Generate(Builder &builder) const override;

        [[nodiscard]] const Type *Decay(Context &context) const override;

        QualifiedType Result;
        std::vector<QualifiedType> Arguments;
        bool Variadic;
    };
}

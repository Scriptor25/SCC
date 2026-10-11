#pragma once

#include <scc/cc/type.hpp>

#include <scc/common.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace scc::cc
{
    class Context
    {
    public:
        explicit Context(const Platform &platform);

        const Platform &GetPlatform() const;

        const VoidType *GetVoidType();

        const IntegerType *GetBooleanType();

        const IntegerType *GetIntegerType(IntegerKind kind);

        const FloatingPointType *GetFloatingPointType(FloatingPointKind kind);

        const StructType *GetStructType(std::string name);
        const StructType *GetStructType(std::vector<StructElement> elements);
        const StructType *GetStructType(
            std::string name,
            std::vector<StructElement> elements);

        const UnionType *GetUnionType(std::string name);
        const UnionType *GetUnionType(std::vector<UnionElement> elements);
        const UnionType *GetUnionType(
            std::string name,
            std::vector<UnionElement> elements);

        const EnumType *GetEnumType(std::string name);
        const EnumType *GetEnumType(std::vector<EnumElement> elements);
        const EnumType *GetEnumType(
            std::string name,
            std::vector<EnumElement> elements);

        const PointerType *GetVoidPointerType(bool is_const);
        const PointerType *GetPointerType(QualifiedType element);

        const ArrayType *GetArrayType(QualifiedType element);
        const ArrayType *GetArrayType(
            QualifiedType element,
            size_t count);

        const FunctionType *GetFunctionType(
            QualifiedType result,
            std::vector<QualifiedType> arguments,
            bool variadic);

        void SetNamedType(const std::string &name, QualifiedType type);
        [[nodiscard]] QualifiedType GetNamedType(const std::string &name) const;

    private:
        const Platform &m_Platform;

        std::unique_ptr<VoidType> m_VoidType;

        std::vector<std::unique_ptr<IntegerType>> m_IntegerTypes;
        std::vector<std::unique_ptr<FloatingPointType>> m_FloatingPointTypes;

        std::vector<std::unique_ptr<StructType>> m_StructTypes;
        std::vector<std::unique_ptr<UnionType>> m_UnionTypes;
        std::vector<std::unique_ptr<EnumType>> m_EnumTypes;

        std::vector<std::unique_ptr<PointerType>> m_PointerTypes;
        std::vector<std::unique_ptr<ArrayType>> m_ArrayTypes;

        std::vector<std::unique_ptr<FunctionType>> m_FunctionTypes;

        std::unordered_map<std::string, QualifiedType> m_NamedTypes;
    };
}

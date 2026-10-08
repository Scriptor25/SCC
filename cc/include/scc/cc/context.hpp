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
        const StructType *GetStructType(std::string name, std::vector<StructElement> elements);
        const UnionType *GetUnionType(std::string name);
        const UnionType *GetUnionType(std::vector<UnionElement> elements);
        const UnionType *GetUnionType(std::string name, std::vector<UnionElement> elements);
        const EnumType *GetEnumType(std::string name);
        const EnumType *GetEnumType(std::vector<EnumElement> elements);
        const EnumType *GetEnumType(std::string name, std::vector<EnumElement> elements);

        const PointerType *GetPointerType();
        const PointerType *GetPointerType(const Type *element);
        const ArrayType *GetArrayType(const Type *element);
        const ArrayType *GetArrayType(const Type *element, size_t count);

        const FunctionType *GetFunctionType(const Type *result, std::vector<const Type *> arguments, bool variadic);

        void SetNamedType(const std::string &name, const Type *type);
        [[nodiscard]] const Type *GetNamedType(const std::string &name) const;

    private:
        const Platform &m_Platform;

        std::unique_ptr<VoidType> m_VoidType;

        std::unordered_map<IntegerKind, std::unique_ptr<IntegerType>> m_IntegerTypes;
        std::unordered_map<FloatingPointKind, std::unique_ptr<FloatingPointType>> m_FloatingPointTypes;

        std::unordered_map<std::string, std::unique_ptr<StructType>> m_NamedStructTypes;
        std::unordered_map<std::string, std::unique_ptr<UnionType>> m_NamedUnionTypes;
        std::unordered_map<std::string, std::unique_ptr<EnumType>> m_NamedEnumTypes;

        std::unordered_map<const Type *, std::unique_ptr<PointerType>> m_PointerTypes;
        std::unordered_map<const Type *, std::unordered_map<size_t, std::unique_ptr<ArrayType>>> m_ArrayTypes;

        std::unordered_map<
            const Type *,
            std::unordered_map<
                bool,
                std::unordered_map<
                    size_t,
                    std::vector<
                        std::unique_ptr<
                            FunctionType
                        >
                    >
                >
            >
        > m_FunctionTypes;

        std::unordered_map<std::string, const Type *> m_NamedTypes;
    };
}

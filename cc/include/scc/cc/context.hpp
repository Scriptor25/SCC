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

        VoidType *GetVoidType();
        IntegerType *GetBooleanType();
        IntegerType *GetIntegerType(IntegerKind kind);
        FloatingPointType *GetFloatingPointType(FloatingPointKind kind);
        StructType *GetStructType(std::string name);
        StructType *GetStructType(std::vector<StructElement> elements);
        UnionType *GetUnionType(std::string name);
        UnionType *GetUnionType(std::vector<UnionElement> elements);
        EnumType *GetEnumType(std::string name);
        EnumType *GetEnumType(std::vector<EnumElement> elements);

        PointerType *GetPointerType();
        PointerType *GetPointerType(Type *element);
        ArrayType *GetArrayType(Type *element);
        ArrayType *GetArrayType(Type *element, size_t count);

        FunctionType *GetFunctionType(Type *result, std::vector<Type *> arguments, bool variadic);

        void SetNamedType(const std::string &name, Type *type);
        [[nodiscard]] Type *GetNamedType(const std::string &name) const;

    private:
        const Platform &m_Platform;

        std::unique_ptr<VoidType> m_VoidType;

        std::unordered_map<IntegerKind, std::unique_ptr<IntegerType>> m_IntegerTypes;
        std::unordered_map<FloatingPointKind, std::unique_ptr<FloatingPointType>> m_FloatingPointTypes;

        std::unordered_map<std::string, std::unique_ptr<StructType>> m_NamedStructTypes;
        std::unordered_map<std::string, std::unique_ptr<UnionType>> m_NamedUnionTypes;
        std::unordered_map<std::string, std::unique_ptr<EnumType>> m_NamedEnumTypes;

        std::unordered_map<Type *, std::unique_ptr<PointerType>> m_PointerTypes;
        std::unordered_map<Type *, std::unordered_map<size_t, std::unique_ptr<ArrayType>>> m_ArrayTypes;

        std::unordered_map<
            Type *,
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

        std::unordered_map<std::string, Type *> m_NamedTypes;
    };
}

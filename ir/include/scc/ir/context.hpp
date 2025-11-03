#pragma once

#include <unordered_map>
#include <vector>
#include <scc/ir/ir.hpp>

namespace scc::ir
{
    class Context final
    {
    public:
        TypePtr GetVoidType();

        TypePtr GetI8Type();
        TypePtr GetI16Type();
        TypePtr GetI32Type();
        TypePtr GetI64Type();

        TypePtr GetF32Type();
        TypePtr GetF64Type();

        TypePtr GetPointerType(TypePtr base);

        TypePtr GetArrayType(TypePtr base, unsigned length);
        TypePtr GetVectorType(TypePtr base, unsigned length);

        TypePtr GetStructType(std::vector<TypePtr> elements);

    private:
        template<typename T, typename... Args> requires std::is_base_of_v<Type, T>
        T::Ptr GetType(Args &&... args)
        {
            auto type = std::make_shared<T>(*this, std::forward<Args>(args)...);
            if (m_Types.contains(type))
            {
                return std::dynamic_pointer_cast<T>(m_Types.at(type));
            }

            m_Types.emplace(type, type);
            return type;
        }

        std::unordered_map<TypePtr, TypePtr, TypeHash> m_Types;
    };
}

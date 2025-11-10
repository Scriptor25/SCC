#pragma once

#include <unordered_map>
#include <vector>
#include <scc/ir/ir.hpp>
#include <scc/ir/type.hpp>

namespace scc::ir
{
    class Context final
    {
    public:
        VoidType::Ptr GetVoidType();

        IntType::Ptr GetI1Type();
        IntType::Ptr GetI8Type();
        IntType::Ptr GetI16Type();
        IntType::Ptr GetI32Type();
        IntType::Ptr GetI64Type();

        FloatType::Ptr GetF32Type();
        FloatType::Ptr GetF64Type();

        PointerType::Ptr GetPointerType(TypeFwd::Ptr base);

        ArrayType::Ptr GetArrayType(TypeFwd::Ptr base, unsigned length);
        VectorType::Ptr GetVectorType(TypeFwd::Ptr base, unsigned length);

        StructType::Ptr GetStructType(std::vector<TypeFwd::Ptr> elements);

        FunctionType::Ptr GetFunctionType(TypeFwd::Ptr result, std::vector<TypeFwd::Ptr> arguments, bool variadic);

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

        std::unordered_map<TypeFwd::Ptr, TypeFwd::Ptr, TypeHash> m_Types;
    };
}

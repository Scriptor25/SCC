#pragma once

#include <scc/ir/constant.hpp>
#include <scc/ir/ir.hpp>
#include <scc/ir/type.hpp>

#include <scc/common.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace scc::ir
{
    class Context final
    {
    public:
        explicit Context(const Platform &platform);
        ~Context();

        Context(const Context &) = delete;
        Context(Context &&context) noexcept;

        Context &operator=(const Context &) = delete;
        Context &operator=(Context &&context) noexcept;

        [[nodiscard]] const Platform &GetPlatform() const;

        VoidType *GetVoidType();

        IntType *GetInt1Type();
        IntType *GetInt8Type();
        IntType *GetInt16Type();
        IntType *GetInt32Type();
        IntType *GetInt64Type();
        IntType *GetIntNType(uint64_t bit_width);

        FloatType *GetFloat32Type();
        FloatType *GetFloat64Type();
        FloatType *GetFloatNType(uint64_t bit_width);

        PointerType *GetPointerType(Type *element);

        ArrayType *GetArrayType(Type *element, uint64_t length);
        VectorType *GetVectorType(Type *element, uint64_t length);

        StructType *GetStructType(std::vector<Type *> elements);

        FunctionType *GetFunctionType(Type *result, std::vector<Type *> arguments, bool variadic);

        ConstantInt *GetInt1(bool value);
        ConstantInt *GetInt8(uint8_t value);
        ConstantInt *GetInt16(uint16_t value);
        ConstantInt *GetInt32(uint32_t value);
        ConstantInt *GetInt64(uint64_t value);
        ConstantInt *GetIntN(uint64_t bit_width, uint64_t value);
        ConstantInt *GetInt(IntType *type, uint64_t value);

        ConstantFloat *GetFloat32(float32_t value);
        ConstantFloat *GetFloat64(float64_t value);
        ConstantFloat *GetFloatN(uint64_t bit_width, float64_t value);
        ConstantFloat *GetFloat(FloatType *type, float64_t value);

        ConstantArray *GetArray(std::vector<Constant *> elements);
        ConstantArray *GetArray(ArrayType *type, std::vector<Constant *> elements);
        ConstantArray *GetArray(std::string_view value);

        ConstantVector *GetVector(std::vector<Constant *> elements);
        ConstantVector *GetVector(VectorType *type, std::vector<Constant *> elements);

        ConstantStruct *GetStruct(std::vector<Constant *> elements);
        ConstantStruct *GetStruct(StructType *type, std::vector<Constant *> elements);

    private:
        template<std::derived_from<Type> T, typename... Args>
        T *Get(Args &&... args)
        {
            auto type = std::make_unique<T>(*this, std::forward<Args>(args)...);
            auto *ptr = type.get();

            for (const auto &entry : m_Types)
                if (entry->Compare(ptr))
                    return dynamic_cast<T *>(entry.get());

            m_Types.push_back(std::move(type));

            return ptr;
        }

        template<std::derived_from<Constant> T, typename... Args>
        T *Get(Args &&... args)
        {
            auto constant = std::make_unique<T>(std::forward<Args>(args)...);
            auto *ptr = constant.get();

            for (const auto &entry : m_Constants)
                if (entry->Compare(ptr))
                    return dynamic_cast<T *>(entry.get());

            m_Constants.push_back(std::move(constant));

            return ptr;
        }

        const Platform *m_Platform;

        std::vector<std::unique_ptr<Type>> m_Types;
        std::vector<std::unique_ptr<Constant>> m_Constants;
    };
}

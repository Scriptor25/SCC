#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::Context::Context(const Platform &platform)
    : m_Platform(&platform)
{
}

scc::ir::Context::~Context()
{
    for (auto &constant : m_Constants)
    {
        constant->ReplaceWith(nullptr);
        constant->DropAll();
        constant.reset();
    }

    m_Constants.clear();
}

scc::ir::Context::Context(Context &&context) noexcept
    : m_Platform(context.m_Platform),
      m_Types(std::move(context.m_Types)),
      m_Constants(std::move(context.m_Constants))
{
}

scc::ir::Context &scc::ir::Context::operator=(Context &&context) noexcept
{
    std::swap(m_Platform, context.m_Platform);
    std::swap(m_Types, context.m_Types);
    std::swap(m_Constants, context.m_Constants);

    return *this;
}

const scc::ir::Platform &scc::ir::Context::GetPlatform() const
{
    return *m_Platform;
}

scc::ir::VoidType *scc::ir::Context::GetVoidType()
{
    return Get<VoidType>();
}

scc::ir::IntType *scc::ir::Context::GetInt1Type()
{
    return Get<IntType>(1);
}

scc::ir::IntType *scc::ir::Context::GetInt8Type()
{
    return Get<IntType>(8);
}

scc::ir::IntType *scc::ir::Context::GetInt16Type()
{
    return Get<IntType>(16);
}

scc::ir::IntType *scc::ir::Context::GetInt32Type()
{
    return Get<IntType>(32);
}

scc::ir::IntType *scc::ir::Context::GetInt64Type()
{
    return Get<IntType>(64);
}

scc::ir::IntType *scc::ir::Context::GetIntNType(uint64_t bit_width)
{
    return Get<IntType>(bit_width);
}

scc::ir::FloatType *scc::ir::Context::GetFloat32Type()
{
    return Get<FloatType>(32);
}

scc::ir::FloatType *scc::ir::Context::GetFloat64Type()
{
    return Get<FloatType>(64);
}

scc::ir::PointerType *scc::ir::Context::GetPointerType(Type *element)
{
    return Get<PointerType>(element);
}

scc::ir::ArrayType *scc::ir::Context::GetArrayType(Type *element, uint64_t length)
{
    Assert(element->IsElement(), "type {} does not qualify as element", element);

    return Get<ArrayType>(element, length);
}

scc::ir::VectorType *scc::ir::Context::GetVectorType(Type *element, uint64_t length)
{
    Assert(element->IsElement(), "type {} does not qualify as element", element);

    return Get<VectorType>(element, length);
}

scc::ir::StructType *scc::ir::Context::GetStructType(std::vector<Type *> elements)
{
    for (auto *element : elements)
        Assert(element->IsElement(), "type {} does not qualify as element", element);

    return Get<StructType>(std::move(elements));
}

scc::ir::FunctionType *scc::ir::Context::GetFunctionType(
    Type *result,
    std::vector<Type *> arguments,
    bool variadic)
{
    return Get<FunctionType>(result, std::move(arguments), variadic);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt1(const bool value)
{
    return Get<ConstantInt>(GetInt1Type(), static_cast<uint64_t>(value) & 0b1);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt8(const uint8_t value)
{
    return Get<ConstantInt>(GetInt8Type(), static_cast<uint64_t>(value));
}

scc::ir::ConstantInt *scc::ir::Context::GetInt16(const uint16_t value)
{
    return Get<ConstantInt>(GetInt16Type(), static_cast<uint64_t>(value));
}

scc::ir::ConstantInt *scc::ir::Context::GetInt32(const uint32_t value)
{
    return Get<ConstantInt>(GetInt32Type(), static_cast<uint64_t>(value));
}

scc::ir::ConstantInt *scc::ir::Context::GetInt64(const uint64_t value)
{
    return Get<ConstantInt>(GetInt64Type(), value);
}

scc::ir::ConstantInt *scc::ir::Context::GetIntN(const uint64_t bit_width, const uint64_t value)
{
    Assert(bit_width >= 1, "bit width must not be less than 1");
    Assert(bit_width <= 64, "bit width must not be greater than 64");

    const auto mask = ~uint64_t() >> (64 - bit_width);
    return Get<ConstantInt>(GetIntNType(bit_width), value & mask);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt(IntType *type, const uint64_t value)
{
    const auto mask = ~uint64_t() >> (64 - type->GetBitWidth());
    return Get<ConstantInt>(type, value & mask);
}

scc::ir::ConstantFloat *scc::ir::Context::GetFloat32(const float32_t value)
{
    return Get<ConstantFloat>(GetFloat32Type(), static_cast<float64_t>(value));
}

scc::ir::ConstantFloat *scc::ir::Context::GetFloat64(const float64_t value)
{
    return Get<ConstantFloat>(GetFloat64Type(), value);
}

scc::ir::ConstantArray *scc::ir::Context::GetArray(std::vector<Constant *> values)
{
    Assert(!values.empty(), "values must not be empty");

    auto *element = values[0]->GetType();

    for (const auto *value : values)
        Assert(element == value->GetType(), "value types must be homogenous");

    return Get<ConstantArray>(GetArrayType(element, values.size()), std::move(values));
}

scc::ir::ConstantArray *scc::ir::Context::GetArray(const std::string_view value)
{
    std::vector<Constant *> values(value.size());

    for (size_t i = 0; i < value.size(); ++i)
        values[i] = GetInt8(value[i]);

    return Get<ConstantArray>(GetArrayType(GetInt8Type(), value.size()), std::move(values));
}

scc::ir::ConstantVector *scc::ir::Context::GetVector(std::vector<Constant *> values)
{
    Assert(!values.empty(), "values must not be empty");

    auto *element = values[0]->GetType();

    for (const auto *value : values)
        Assert(element == value->GetType(), "value types must be homogenous");

    return Get<ConstantVector>(GetVectorType(element, values.size()), std::move(values));
}

scc::ir::ConstantStruct *scc::ir::Context::GetStruct(std::vector<Constant *> values)
{
    std::vector<Type *> elements(values.size());

    for (size_t i = 0; i < values.size(); ++i)
        elements[i] = values[i]->GetType();

    return Get<ConstantStruct>(GetStructType(std::move(elements)), std::move(values));
}

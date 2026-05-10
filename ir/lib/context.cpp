#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

#include <scc/platform.hpp>

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

const scc::Platform &scc::ir::Context::GetPlatform() const
{
    return *m_Platform;
}

scc::ir::VoidType *scc::ir::Context::GetVoidType()
{
    return Get<VoidType>();
}

scc::ir::IntType *scc::ir::Context::GetInt1Type()
{
    return GetIntNType(1);
}

scc::ir::IntType *scc::ir::Context::GetInt8Type()
{
    return GetIntNType(8);
}

scc::ir::IntType *scc::ir::Context::GetInt16Type()
{
    return GetIntNType(16);
}

scc::ir::IntType *scc::ir::Context::GetInt32Type()
{
    return GetIntNType(32);
}

scc::ir::IntType *scc::ir::Context::GetInt64Type()
{
    return GetIntNType(64);
}

scc::ir::IntType *scc::ir::Context::GetIntNType(uint64_t bit_width)
{
    Assert(
        m_Platform->IR.LegalIntWidth.contains(bit_width),
        "bit width {} is not legal for current platform",
        bit_width);

    return Get<IntType>(bit_width);
}

scc::ir::FloatType *scc::ir::Context::GetFloat32Type()
{
    return GetFloatNType(32);
}

scc::ir::FloatType *scc::ir::Context::GetFloat64Type()
{
    return GetFloatNType(64);
}

scc::ir::FloatType *scc::ir::Context::GetFloatNType(uint64_t bit_width)
{
    Assert(
        m_Platform->IR.LegalFloatWidth.contains(bit_width),
        "bit width {} is not legal for current platform",
        bit_width);

    return Get<FloatType>(bit_width);
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
    return GetIntN(1, value);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt8(const uint8_t value)
{
    return GetIntN(8, value);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt16(const uint16_t value)
{
    return GetIntN(16, value);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt32(const uint32_t value)
{
    return GetIntN(32, value);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt64(const uint64_t value)
{
    return GetIntN(64, value);
}

scc::ir::ConstantInt *scc::ir::Context::GetIntN(const uint64_t bit_width, const uint64_t value)
{
    return GetInt(GetIntNType(bit_width), value);
}

scc::ir::ConstantInt *scc::ir::Context::GetInt(IntType *type, const uint64_t value)
{
    const auto mask = ~uint64_t() >> (64 - type->GetBitWidth());
    return Get<ConstantInt>(type, value & mask);
}

scc::ir::ConstantFloat *scc::ir::Context::GetFloat32(const float32_t value)
{
    return GetFloatN(32, value);
}

scc::ir::ConstantFloat *scc::ir::Context::GetFloat64(const float64_t value)
{
    return GetFloatN(64, value);
}

scc::ir::ConstantFloat *scc::ir::Context::GetFloatN(const uint64_t bit_width, const float64_t value)
{
    return GetFloat(GetFloatNType(bit_width), value);
}

scc::ir::ConstantFloat *scc::ir::Context::GetFloat(FloatType *type, float64_t value)
{
    return Get<ConstantFloat>(type, value);
}

scc::ir::ConstantArray *scc::ir::Context::GetArray(std::vector<Constant *> elements)
{
    Assert(!elements.empty(), "elements must not be empty");

    auto *element_type = elements[0]->GetType();

    for (const auto *element : elements)
        Assert(
            element->GetType() == element_type,
            "invalid element type: is {}, requires {}",
            element->GetType(),
            element_type);

    return Get<ConstantArray>(GetArrayType(element_type, elements.size()), std::move(elements));
}

scc::ir::ConstantArray *scc::ir::Context::GetArray(ArrayType *type, std::vector<Constant *> elements)
{
    Assert(
        elements.size() != type->GetElementCount(),
        "invalid element count: is {}, requires {}",
        elements.size(),
        type->GetElementCount());

    auto *element_type = type->GetElement();

    for (const auto *element : elements)
        Assert(
            element->GetType() == element_type,
            "invalid element type: is {}, requires {}",
            element->GetType(),
            element_type);

    return Get<ConstantArray>(type, std::move(elements));
}

scc::ir::ConstantArray *scc::ir::Context::GetArray(const std::string_view value)
{
    std::vector<Constant *> elements(value.size());

    for (size_t i = 0; i < value.size(); ++i)
        elements[i] = GetInt8(value[i]);

    return Get<ConstantArray>(GetArrayType(GetInt8Type(), elements.size()), std::move(elements));
}

scc::ir::ConstantVector *scc::ir::Context::GetVector(std::vector<Constant *> elements)
{
    Assert(!elements.empty(), "elements must not be empty");

    auto *element_type = elements[0]->GetType();

    for (const auto *element : elements)
        Assert(
            element->GetType() == element_type,
            "invalid element type: is {}, requires {}",
            element->GetType(),
            element_type);

    return Get<ConstantVector>(GetVectorType(element_type, elements.size()), std::move(elements));
}

scc::ir::ConstantVector *scc::ir::Context::GetVector(VectorType *type, std::vector<Constant *> elements)
{
    Assert(
        elements.size() != type->GetElementCount(),
        "invalid element count: is {}, requires {}",
        elements.size(),
        type->GetElementCount());

    auto *element_type = type->GetElement();

    for (const auto *element : elements)
        Assert(
            element->GetType() == element_type,
            "invalid element type: is {}, requires {}",
            element->GetType(),
            element_type);

    return Get<ConstantVector>(type, std::move(elements));
}

scc::ir::ConstantStruct *scc::ir::Context::GetStruct(std::vector<Constant *> elements)
{
    std::vector<Type *> element_types(elements.size());

    for (size_t i = 0; i < elements.size(); ++i)
        element_types[i] = elements[i]->GetType();

    return Get<ConstantStruct>(GetStructType(std::move(element_types)), std::move(elements));
}

scc::ir::ConstantStruct *scc::ir::Context::GetStruct(StructType *type, std::vector<Constant *> elements)
{
    Assert(
        elements.size() != type->GetElementCount(),
        "invalid element count: is {}, requires {}",
        elements.size(),
        type->GetElementCount());

    for (size_t i = 0; i < elements.size(); ++i)
        Assert(
            elements[i]->GetType() == type->GetElement(i),
            "invalid element type: is {}, requires {}",
            elements[i]->GetType(),
            type->GetElement(i));

    return Get<ConstantStruct>(type, std::move(elements));
}

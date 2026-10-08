#include <scc/cc/context.hpp>

#include <scc/assert.hpp>

scc::cc::Context::Context(const Platform &platform)
    : m_Platform(platform)
{
}

const scc::Platform &scc::cc::Context::GetPlatform() const
{
    return m_Platform;
}

const scc::cc::VoidType *scc::cc::Context::GetVoidType()
{
    if (!m_VoidType)
        m_VoidType = std::make_unique<VoidType>();

    return m_VoidType.get();
}

const scc::cc::IntegerType *scc::cc::Context::GetBooleanType()
{
    return GetIntegerType(IntegerKind::Bool);
}

const scc::cc::IntegerType *scc::cc::Context::GetIntegerType(IntegerKind kind)
{
    auto &ref = m_IntegerTypes[kind];

    if (!ref)
        ref = std::make_unique<IntegerType>(kind);

    return ref.get();
}

const scc::cc::FloatingPointType *scc::cc::Context::GetFloatingPointType(FloatingPointKind kind)
{
    auto &ref = m_FloatingPointTypes[kind];

    if (!ref)
        ref = std::make_unique<FloatingPointType>(kind);

    return ref.get();
}

const scc::cc::StructType *scc::cc::Context::GetStructType(std::string name)
{
    auto &ref = m_NamedStructTypes[name];

    if (!ref)
        ref = std::make_unique<StructType>(std::move(name));

    return ref.get();
}

const scc::cc::StructType *scc::cc::Context::GetStructType(std::vector<StructElement> elements)
{
    Error("TODO");
}

const scc::cc::StructType *scc::cc::Context::GetStructType(std::string name, std::vector<StructElement> elements)
{
    Error("TODO");
}

const scc::cc::UnionType *scc::cc::Context::GetUnionType(std::string name)
{
    auto &ref = m_NamedUnionTypes[name];

    if (!ref)
        ref = std::make_unique<UnionType>(std::move(name));

    return ref.get();
}

const scc::cc::UnionType *scc::cc::Context::GetUnionType(std::vector<UnionElement> elements)
{
    Error("TODO");
}

const scc::cc::UnionType *scc::cc::Context::GetUnionType(std::string name, std::vector<UnionElement> elements)
{
    Error("TODO");
}

const scc::cc::EnumType *scc::cc::Context::GetEnumType(std::string name)
{
    auto &ref = m_NamedEnumTypes[name];

    if (!ref)
        ref = std::make_unique<EnumType>(std::move(name));

    return ref.get();
}

const scc::cc::EnumType *scc::cc::Context::GetEnumType(std::vector<EnumElement> elements)
{
    Error("TODO");
}

const scc::cc::EnumType *scc::cc::Context::GetEnumType(std::string name, std::vector<EnumElement> elements)
{
    Error("TODO");
}

const scc::cc::PointerType *scc::cc::Context::GetPointerType()
{
    return GetPointerType(nullptr);
}

const scc::cc::PointerType *scc::cc::Context::GetPointerType(const Type *element)
{
    auto &ref = m_PointerTypes[element];

    if (!ref)
        ref = std::make_unique<PointerType>(element);

    return ref.get();
}

const scc::cc::ArrayType *scc::cc::Context::GetArrayType(const Type *element)
{
    return GetArrayType(element, 0);
}

const scc::cc::ArrayType *scc::cc::Context::GetArrayType(const Type *element, size_t count)
{
    auto &ref = m_ArrayTypes[element][count];

    if (!ref)
        ref = std::make_unique<ArrayType>(element, count);

    return ref.get();
}

const scc::cc::FunctionType *scc::cc::Context::GetFunctionType(
    const Type *result,
    std::vector<const Type *> arguments,
    bool variadic)
{
    auto &types = m_FunctionTypes[result][variadic][arguments.size()];
    for (const auto &type : types)
    {
        size_t i;
        for (i = 0; i < arguments.size(); ++i)
            if (type->Arguments[i] != arguments[i])
                break;
        if (i < arguments.size())
            continue;

        return type.get();
    }

    auto type = std::make_unique<FunctionType>(result, std::move(arguments), variadic);

    auto *ptr = type.get();

    types.push_back(std::move(type));

    return ptr;
}

void scc::cc::Context::SetNamedType(const std::string &name, const Type *type)
{
    m_NamedTypes[name] = type;
}

const scc::cc::Type *scc::cc::Context::GetNamedType(const std::string &name) const
{
    if (const auto it = m_NamedTypes.find(name); it != m_NamedTypes.end())
        return it->second;
    return nullptr;
}

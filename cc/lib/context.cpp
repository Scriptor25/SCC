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
    for (const auto &type : m_IntegerTypes)
        if (type->IKind == kind)
            return type.get();

    auto type = std::make_unique<IntegerType>(kind);

    const auto *ptr = type.get();
    m_IntegerTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::FloatingPointType *scc::cc::Context::GetFloatingPointType(FloatingPointKind kind)
{
    for (const auto &type : m_FloatingPointTypes)
        if (type->FKind == kind)
            return type.get();

    auto type = std::make_unique<FloatingPointType>(kind);

    const auto *ptr = type.get();
    m_FloatingPointTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::StructType *scc::cc::Context::GetStructType(std::string name)
{
    for (const auto &type : m_StructTypes)
        if (type->Name == name)
            return type.get();

    auto type = std::make_unique<StructType>(std::move(name));

    const auto *ptr = type.get();
    m_StructTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::StructType *scc::cc::Context::GetStructType(std::vector<StructElement> elements)
{
    for (const auto &type : m_StructTypes)
    {
        if (type->Name)
            continue;
        if (type->Elements.size() != elements.size())
            continue;

        size_t i;
        for (i = 0; i < type->Elements.size(); ++i)
            if (type->Elements[i] != elements[i])
                break;

        if (i == type->Elements.size())
            return type.get();
    }

    auto type = std::make_unique<StructType>(std::move(elements));

    const auto *ptr = type.get();
    m_StructTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::StructType *scc::cc::Context::GetStructType(
    std::string name,
    std::vector<StructElement> elements)
{
    for (const auto &type : m_StructTypes)
        if (type->Name == name)
        {
            Assert(type->Elements.empty(), "type elements must be empty");
            type->Elements = std::move(elements);
            return type.get();
        }

    auto type = std::make_unique<StructType>(std::move(name), std::move(elements));

    const auto *ptr = type.get();
    m_StructTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::UnionType *scc::cc::Context::GetUnionType(std::string name)
{
    for (const auto &type : m_UnionTypes)
        if (type->Name == name)
            return type.get();

    auto type = std::make_unique<UnionType>(std::move(name));

    const auto *ptr = type.get();
    m_UnionTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::UnionType *scc::cc::Context::GetUnionType(std::vector<UnionElement> elements)
{
    for (const auto &type : m_UnionTypes)
    {
        if (type->Name)
            continue;
        if (type->Elements.size() != elements.size())
            continue;

        size_t i;
        for (i = 0; i < type->Elements.size(); ++i)
            if (type->Elements[i] != elements[i])
                break;

        if (i == type->Elements.size())
            return type.get();
    }

    auto type = std::make_unique<UnionType>(std::move(elements));

    const auto *ptr = type.get();
    m_UnionTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::UnionType *scc::cc::Context::GetUnionType(
    std::string name,
    std::vector<UnionElement> elements)
{
    for (const auto &type : m_UnionTypes)
        if (type->Name == name)
        {
            Assert(type->Elements.empty(), "type elements must be empty");
            type->Elements = std::move(elements);
            return type.get();
        }

    auto type = std::make_unique<UnionType>(std::move(name), std::move(elements));

    const auto *ptr = type.get();
    m_UnionTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::EnumType *scc::cc::Context::GetEnumType(std::string name)
{
    for (const auto &type : m_EnumTypes)
        if (type->Name == name)
            return type.get();

    auto type = std::make_unique<EnumType>(std::move(name));

    const auto *ptr = type.get();
    m_EnumTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::EnumType *scc::cc::Context::GetEnumType(std::vector<EnumElement> elements)
{
    for (const auto &type : m_EnumTypes)
    {
        if (type->Name)
            continue;
        if (type->Elements.size() != elements.size())
            continue;

        size_t i;
        for (i = 0; i < type->Elements.size(); ++i)
            if (type->Elements[i] != elements[i])
                break;

        if (i == type->Elements.size())
            return type.get();
    }

    auto type = std::make_unique<EnumType>(std::move(elements));

    const auto *ptr = type.get();
    m_EnumTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::EnumType *scc::cc::Context::GetEnumType(
    std::string name,
    std::vector<EnumElement> elements)
{
    for (const auto &type : m_EnumTypes)
        if (type->Name == name)
        {
            Assert(type->Elements.empty(), "type elements must be empty");
            type->Elements = std::move(elements);
            return type.get();
        }

    auto type = std::make_unique<EnumType>(std::move(name), std::move(elements));

    const auto *ptr = type.get();
    m_EnumTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::PointerType *scc::cc::Context::GetVoidPointerType(const bool is_const)
{
    return GetPointerType(
        {
            .Base = GetVoidType(),
            .Const = is_const,
        });
}

const scc::cc::PointerType *scc::cc::Context::GetPointerType(QualifiedType element)
{
    for (const auto &type : m_PointerTypes)
        if (type->Element == element)
            return type.get();

    auto type = std::make_unique<PointerType>(element);

    const auto *ptr = type.get();
    m_PointerTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::ArrayType *scc::cc::Context::GetArrayType(const QualifiedType element)
{
    return GetArrayType(element, ~size_t{});
}

const scc::cc::ArrayType *scc::cc::Context::GetArrayType(
    QualifiedType element,
    size_t count)
{
    for (const auto &type : m_ArrayTypes)
        if (type->Element == element && type->Count == count)
            return type.get();

    auto type = std::make_unique<ArrayType>(element, count);

    const auto *ptr = type.get();
    m_ArrayTypes.push_back(std::move(type));
    return ptr;
}

const scc::cc::FunctionType *scc::cc::Context::GetFunctionType(
    QualifiedType result,
    std::vector<QualifiedType> arguments,
    bool variadic)
{
    for (const auto &type : m_FunctionTypes)
    {
        if (type->Result != result)
            continue;
        if (type->Variadic != variadic)
            continue;
        if (type->Arguments.size() != arguments.size())
            continue;

        size_t i;
        for (i = 0; i < type->Arguments.size(); ++i)
            if (type->Arguments[i] != arguments[i])
                break;

        if (i == type->Arguments.size())
            return type.get();
    }

    auto type = std::make_unique<FunctionType>(result, std::move(arguments), variadic);

    const auto *ptr = type.get();
    m_FunctionTypes.push_back(std::move(type));
    return ptr;
}

void scc::cc::Context::SetNamedType(const std::string &name, QualifiedType type)
{
    m_NamedTypes[name] = type;
}

scc::cc::QualifiedType scc::cc::Context::GetNamedType(const std::string &name) const
{
    if (const auto it = m_NamedTypes.find(name); it != m_NamedTypes.end())
        return it->second;
    return {};
}

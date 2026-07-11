#include <scc/cc/context.hpp>

#include <scc/assert.hpp>

scc::cc::Context::Context(ir::Context &ir_context)
    : m_IRContext(ir_context)
{
}

scc::ir::Context &scc::cc::Context::GetIRContext() const
{
    return m_IRContext;
}

scc::cc::VoidType *scc::cc::Context::GetVoidType()
{
    if (!m_VoidType)
        m_VoidType = std::make_unique<VoidType>();

    return m_VoidType.get();
}

scc::cc::IntegerType *scc::cc::Context::GetBooleanType()
{
    return GetIntegerType(IntegerKind::Bool);
}

scc::cc::IntegerType *scc::cc::Context::GetIntegerType(IntegerKind kind)
{
    auto &ref = m_IntegerTypes[kind];

    if (!ref)
        ref = std::make_unique<IntegerType>(kind);

    return ref.get();
}

scc::cc::FloatingPointType *scc::cc::Context::GetFloatingPointType(FloatingPointKind kind)
{
    auto &ref = m_FloatingPointTypes[kind];

    if (!ref)
        ref = std::make_unique<FloatingPointType>(kind);

    return ref.get();
}

scc::cc::StructType *scc::cc::Context::GetStructType(std::string name)
{
    auto &ref = m_NamedStructTypes[name];

    if (!ref)
        ref = std::make_unique<StructType>(std::move(name));

    return ref.get();
}

scc::cc::StructType *scc::cc::Context::GetStructType(std::vector<StructElement> elements)
{
    Error("TODO");
}

scc::cc::UnionType *scc::cc::Context::GetUnionType(std::string name)
{
    auto &ref = m_NamedUnionTypes[name];

    if (!ref)
        ref = std::make_unique<UnionType>(std::move(name));

    return ref.get();
}

scc::cc::UnionType *scc::cc::Context::GetUnionType(std::vector<UnionElement> elements)
{
    Error("TODO");
}

scc::cc::EnumType *scc::cc::Context::GetEnumType(std::string name)
{
    auto &ref = m_NamedEnumTypes[name];

    if (!ref)
        ref = std::make_unique<EnumType>(std::move(name));

    return ref.get();
}

scc::cc::EnumType *scc::cc::Context::GetEnumType(std::vector<EnumElement> elements)
{
    Error("TODO");
}

scc::cc::PointerType *scc::cc::Context::GetPointerType()
{
    return GetPointerType(nullptr);
}

scc::cc::PointerType *scc::cc::Context::GetPointerType(Type *element)
{
    auto &ref = m_PointerTypes[element];

    if (!ref)
        ref = std::make_unique<PointerType>(element);

    return ref.get();
}

scc::cc::ArrayType *scc::cc::Context::GetArrayType(Type *element)
{
    return GetArrayType(element, 0);
}

scc::cc::ArrayType *scc::cc::Context::GetArrayType(Type *element, size_t count)
{
    auto &ref = m_ArrayTypes[element][count];

    if (!ref)
        ref = std::make_unique<ArrayType>(element, count);

    return ref.get();
}

scc::cc::Type *scc::cc::Context::GetNamedType(const std::string &name) const
{
    if (const auto it = m_NamedTypes.find(name); it != m_NamedTypes.end())
        return it->second;
    return nullptr;
}

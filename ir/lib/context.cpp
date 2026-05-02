#include <scc/assert.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

scc::ir::Context::Context(const Platform &platform)
    : m_Platform(platform)
{
}

const scc::ir::Platform &scc::ir::Context::GetPlatform() const
{
    return m_Platform;
}

scc::ir::VoidType::Ptr scc::ir::Context::GetVoidType()
{
    return GetType<VoidType>();
}

scc::ir::IntType::Ptr scc::ir::Context::GetI1Type()
{
    return GetType<IntType>(1u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI8Type()
{
    return GetType<IntType>(8u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI16Type()
{
    return GetType<IntType>(16u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI32Type()
{
    return GetType<IntType>(32u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI64Type()
{
    return GetType<IntType>(64u);
}

scc::ir::FloatType::Ptr scc::ir::Context::GetF32Type()
{
    return GetType<FloatType>(32u);
}

scc::ir::FloatType::Ptr scc::ir::Context::GetF64Type()
{
    return GetType<FloatType>(64u);
}

scc::ir::PointerType::Ptr scc::ir::Context::GetPointerType(TypeFwd::Ptr element)
{
    return GetType<PointerType>(std::move(element));
}

scc::ir::ArrayType::Ptr scc::ir::Context::GetArrayType(TypeFwd::Ptr element, unsigned length)
{
    Assert(element->IsElement(), "type {} does not qualify as element", element);
    return GetType<ArrayType>(std::move(element), length);
}

scc::ir::VectorType::Ptr scc::ir::Context::GetVectorType(TypeFwd::Ptr element, unsigned length)
{
    Assert(element->IsElement(), "type {} does not qualify as element", element);
    return GetType<VectorType>(std::move(element), length);
}

scc::ir::StructType::Ptr scc::ir::Context::GetStructType(std::vector<TypeFwd::Ptr> elements)
{
    for (auto &element : elements)
        Assert(element->IsElement(), "type {} does not qualify as element", element);
    return GetType<StructType>(std::move(elements));
}

scc::ir::FunctionType::Ptr scc::ir::Context::GetFunctionType(
    TypeFwd::Ptr result,
    std::vector<TypeFwd::Ptr> arguments,
    bool variadic)
{
    return GetType<FunctionType>(std::move(result), std::move(arguments), variadic);
}

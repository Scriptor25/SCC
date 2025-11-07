#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

scc::ir::VoidType::Ptr scc::ir::Context::GetVoidType()
{
    return GetType<VoidType>();
}

scc::ir::Shared<scc::ir::IntType>::Ptr scc::ir::Context::GetI1Type()
{
    return GetType<IntType>(0u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI8Type()
{
    return GetType<IntType>(1u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI16Type()
{
    return GetType<IntType>(2u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI32Type()
{
    return GetType<IntType>(4u);
}

scc::ir::IntType::Ptr scc::ir::Context::GetI64Type()
{
    return GetType<IntType>(8u);
}

scc::ir::FloatType::Ptr scc::ir::Context::GetF32Type()
{
    return GetType<FloatType>(4u);
}

scc::ir::FloatType::Ptr scc::ir::Context::GetF64Type()
{
    return GetType<FloatType>(8u);
}

scc::ir::PointerType::Ptr scc::ir::Context::GetPointerType(TypePtr base)
{
    return GetType<PointerType>(std::move(base));
}

scc::ir::ArrayType::Ptr scc::ir::Context::GetArrayType(TypePtr base, unsigned length)
{
    return GetType<ArrayType>(std::move(base), length);
}

scc::ir::VectorType::Ptr scc::ir::Context::GetVectorType(TypePtr base, unsigned length)
{
    return GetType<VectorType>(std::move(base), length);
}

scc::ir::StructType::Ptr scc::ir::Context::GetStructType(std::vector<TypePtr> elements)
{
    return GetType<StructType>(std::move(elements));
}

scc::ir::Shared<scc::ir::FunctionType>::Ptr scc::ir::Context::GetFunctionType(
    TypePtr result,
    std::vector<TypePtr> arguments,
    bool variadic)
{
    return GetType<FunctionType>(std::move(result), std::move(arguments), variadic);
}

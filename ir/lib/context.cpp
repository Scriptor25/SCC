#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

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

scc::ir::PointerType::Ptr scc::ir::Context::GetPointerType(TypeFwd::Ptr base)
{
    return GetType<PointerType>(std::move(base));
}

scc::ir::ArrayType::Ptr scc::ir::Context::GetArrayType(TypeFwd::Ptr base, unsigned length)
{
    return GetType<ArrayType>(std::move(base), length);
}

scc::ir::VectorType::Ptr scc::ir::Context::GetVectorType(TypeFwd::Ptr base, unsigned length)
{
    return GetType<VectorType>(std::move(base), length);
}

scc::ir::StructType::Ptr scc::ir::Context::GetStructType(std::vector<TypeFwd::Ptr> elements)
{
    return GetType<StructType>(std::move(elements));
}

scc::ir::FunctionType::Ptr scc::ir::Context::GetFunctionType(
    TypeFwd::Ptr result,
    std::vector<TypeFwd::Ptr> arguments,
    bool variadic)
{
    return GetType<FunctionType>(std::move(result), std::move(arguments), variadic);
}

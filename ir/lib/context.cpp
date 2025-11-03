#include <scc/ir/context.hpp>
#include <scc/ir/type.hpp>

scc::ir::TypePtr scc::ir::Context::GetVoidType()
{
    return GetType<VoidType>();
}

scc::ir::TypePtr scc::ir::Context::GetI8Type()
{
    return GetType<IntType>(1u);
}

scc::ir::TypePtr scc::ir::Context::GetI16Type()
{
    return GetType<IntType>(2u);
}

scc::ir::TypePtr scc::ir::Context::GetI32Type()
{
    return GetType<IntType>(4u);
}

scc::ir::TypePtr scc::ir::Context::GetI64Type()
{
    return GetType<IntType>(8u);
}

scc::ir::TypePtr scc::ir::Context::GetF32Type()
{
    return GetType<FloatType>(4u);
}

scc::ir::TypePtr scc::ir::Context::GetF64Type()
{
    return GetType<FloatType>(8u);
}

scc::ir::TypePtr scc::ir::Context::GetPointerType(TypePtr base)
{
    return GetType<PointerType>(std::move(base));
}

scc::ir::TypePtr scc::ir::Context::GetArrayType(TypePtr base, unsigned length)
{
    return GetType<ArrayType>(std::move(base), length);
}

scc::ir::TypePtr scc::ir::Context::GetVectorType(TypePtr base, unsigned length)
{
    return GetType<VectorType>(std::move(base), length);
}

scc::ir::TypePtr scc::ir::Context::GetStructType(std::vector<TypePtr> elements)
{
    return GetType<StructType>(std::move(elements));
}

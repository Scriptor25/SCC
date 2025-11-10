#include <iostream>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/module.hpp>
#include <scc/ir/value.hpp>

int main()
{
    scc::ir::Module module;
    scc::ir::Context context;
    scc::ir::Builder builder(context);

    const auto format = builder.CreateString(module, "format", "fib(%d) = %d\r\n");

    const auto atoi_function = module.CreateFunction(
        context.GetFunctionType(context.GetI32Type(), { context.GetPointerType(context.GetI8Type()) }, false),
        "atoi");
    const auto printf_function = module.CreateFunction(
        context.GetFunctionType(context.GetI32Type(), { context.GetPointerType(context.GetI8Type()) }, true),
        "printf");

    const auto fib_function = module.CreateFunction(
        context.GetFunctionType(context.GetI32Type(), { context.GetI32Type() }, false),
        "fib");
    {
        const auto entry_block = builder.CreateBlock(fib_function, "entry");
        const auto head_block = builder.CreateBlock(fib_function, "head");
        const auto loop_block = builder.CreateBlock(fib_function, "loop");
        const auto end_block = builder.CreateBlock(fib_function, "end");

        const auto n = fib_function->GetArgument(0);
        n->SetName("n");

        builder.SetInsertBlock(entry_block);
        auto pt = builder.CreateAlloc(context.GetI32Type(), "pt", 3);
        auto ap = builder.CreateConstOffset(pt, { 0u }, "ap");
        auto bp = builder.CreateConstOffset(pt, { 1u }, "bp");
        auto ip = builder.CreateConstOffset(pt, { 2u }, "ip");
        builder.CreateStore(ap, builder.CreateI32(0));
        builder.CreateStore(bp, builder.CreateI32(1));
        builder.CreateStore(ip, builder.CreateI32(0));
        builder.CreateBranch(head_block);

        builder.SetInsertBlock(head_block);
        auto i = builder.CreateLoad(ip, "i");
        auto lt = builder.CreateLT(i, n, "lt");
        builder.CreateBranch(lt, loop_block, end_block);

        builder.SetInsertBlock(loop_block);
        auto a = builder.CreateLoad(ap, "a");
        auto b = builder.CreateLoad(bp, "b");
        auto c = builder.CreateAdd({ a, b }, "c");
        builder.CreateStore(ap, b);
        builder.CreateStore(bp, c);
        auto x = builder.CreateAdd({ i, builder.CreateI32(1) }, "x");
        builder.CreateStore(ip, x);
        builder.CreateBranch(head_block);

        builder.SetInsertBlock(end_block);
        builder.CreateRet(a);

        builder.ClearInsertBlock();
    }

    const auto main_function = module.CreateFunction(
        context.GetFunctionType(
            context.GetI32Type(),
            { context.GetI32Type(), context.GetPointerType(context.GetPointerType(context.GetI8Type())) },
            false),
        "main");
    {
        const auto entry_block = builder.CreateBlock(main_function, "entry");

        builder.SetInsertBlock(entry_block);

        const auto argc = main_function->GetArgument(0);
        argc->SetName("argc");
        const auto argv = main_function->GetArgument(1);
        argv->SetName("argv");

        auto np = builder.CreateConstOffset(argv, { 1u }, "np");
        auto ns = builder.CreateLoad(np, "ns");
        auto n = builder.CreateCall(atoi_function, { ns }, "n");
        auto r = builder.CreateCall(fib_function, { n }, "r");
        auto fp = builder.CreateCast(context.GetPointerType(context.GetI8Type()), format, "fp");
        builder.CreateCall(printf_function, { fp, n, r });
        builder.CreateRet(r);

        builder.ClearInsertBlock();
    }

    module.Print(std::cout);
}

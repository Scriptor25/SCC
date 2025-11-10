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
        auto pt = builder.CreateAlloc("pt", context.GetI32Type(), 3);
        auto ap = builder.CreateConstOffset("ap", pt, 0u);
        auto bp = builder.CreateConstOffset("bp", pt, 1u);
        auto ip = builder.CreateConstOffset("ip", pt, 2u);
        builder.CreateStore(ap, builder.CreateI32(0));
        builder.CreateStore(bp, builder.CreateI32(1));
        builder.CreateStore(ip, builder.CreateI32(0));
        builder.CreateBranch(head_block);

        builder.SetInsertBlock(head_block);
        auto i = builder.CreateLoad("i", ip);
        auto lt = builder.CreateComparator("lt", scc::ir::Comparator_LT, i, n);
        builder.CreateBranch(lt, loop_block, end_block);

        builder.SetInsertBlock(loop_block);
        auto a = builder.CreateLoad("a", ap);
        auto b = builder.CreateLoad("b", bp);
        auto c = builder.CreateOperator("c", scc::ir::Operator_Add, a, b);
        builder.CreateStore(ap, b);
        builder.CreateStore(bp, c);
        auto x = builder.CreateOperator("x", scc::ir::Operator_Add, i, builder.CreateI32(1));
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

        auto np = builder.CreateConstOffset("np", argv, 1u);
        auto ns = builder.CreateLoad("ns", np);
        auto n = builder.CreateCall("n", atoi_function, ns);
        auto r = builder.CreateCall("r", fib_function, n);
        auto fp = builder.CreateCast("fp", format, context.GetPointerType(context.GetI8Type()));
        builder.CreateCallVoid(printf_function, fp, n, r);
        builder.CreateRet(r);

        builder.ClearInsertBlock();
    }

    module.Print(std::cout);
}

#include <scc/ir/argument.hpp>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/function.hpp>
#include <scc/ir/parser.hpp>

#include <scc/assert.hpp>
#include <scc/error.hpp>

#include <iostream>
#include <istream>
#include <map>
#include <unordered_map>

scc::ir::Parser::Parser(std::istream &stream)
    : m_Stream(stream)
{
    m_Buffer = m_Stream.get();
    Next();
}

int scc::ir::Parser::Get()
{
    return m_Buffer = m_Stream.get();
}

static bool isdigit(const unsigned base, const int c)
{
    switch (base)
    {
    case 0b10:
        return '0' <= c && c <= '1';
    case 010:
        return '0' <= c && c <= '7';
    case 10:
        return '0' <= c && c <= '9';
    case 0x10:
        return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
    default:
        return false;
    }
}

scc::ir::Token &scc::ir::Parser::Next()
{
    enum class State
    {
        None,
        Comment,
        Integer,
        Identifier,
        String,
    };

    std::string value;

    auto state = State::None;
    auto base = 0u;

    while (m_Buffer >= 0)
    {
        switch (state)
        {
        case State::None:
            switch (m_Buffer)
            {
            case '\n':
                Get();
                return m_Token = { .Type = TokenType::EndOfLine };

            case ';':
                state = State::Comment;
                break;

            case '0':
                Get();
                switch (m_Buffer)
                {
                case 'b':
                    Get();
                    state = State::Integer;
                    base = 0b10u;
                    break;
                case 'x':
                    Get();
                    state = State::Integer;
                    base = 0x10u;
                    break;
                default:
                    value += '0';
                    state = State::Integer;
                    base = 010u;
                    break;
                }
                break;

            case '"':
                Get();
                state = State::String;
                break;

            case '.':
                value += static_cast<char>(m_Buffer);
                Get();
                while (m_Buffer == '.')
                {
                    value += static_cast<char>(m_Buffer);
                    Get();
                }
                return m_Token = { .Type = TokenType::Other, .Value = std::move(value) };

            default:
                if (std::isspace(m_Buffer))
                {
                    Get();
                    break;
                }

                if (std::isdigit(m_Buffer))
                {
                    state = State::Integer;
                    base = 10u;
                    break;
                }

                if (std::isalnum(m_Buffer))
                {
                    state = State::Identifier;
                    break;
                }

                value += static_cast<char>(m_Buffer);
                Get();
                return m_Token = { .Type = TokenType::Other, .Value = std::move(value) };
            }
            break;

        case State::Comment:
            if (m_Buffer == '\n')
            {
                state = State::None;
                break;
            }

            Get();
            break;

        case State::Integer:
            if (isdigit(base, m_Buffer))
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            {
                const auto int_value = std::stoull(value, nullptr, static_cast<int>(base));
                return m_Token = { .Type = TokenType::Integer, .Value = std::move(value), .IntValue = int_value };
            }

        case State::Identifier:
            if (std::isalnum(m_Buffer))
            {
                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            return m_Token = { .Type = TokenType::Identifier, .Value = std::move(value) };

        case State::String:
            if (m_Buffer != '"')
            {
                if (m_Buffer == '\\')
                {
                    Get();
                    switch (m_Buffer)
                    {
                    case 'a':
                        m_Buffer = '\a';
                        break;
                    case 'b':
                        m_Buffer = '\b';
                        break;
                    case 'f':
                        m_Buffer = '\f';
                        break;
                    case 'n':
                        m_Buffer = '\n';
                        break;
                    case 'r':
                        m_Buffer = '\r';
                        break;
                    case 't':
                        m_Buffer = '\t';
                        break;
                    case 'v':
                        m_Buffer = '\v';
                        break;
                    case 'x':
                    {
                        char buffer[3];

                        Get();
                        buffer[0] = static_cast<char>(m_Buffer);
                        Get();
                        buffer[1] = static_cast<char>(m_Buffer);
                        buffer[2] = 0;

                        m_Buffer = std::stoi(buffer, nullptr, 0x10);
                        break;
                    }
                    default:
                        break;
                    }
                }

                value += static_cast<char>(m_Buffer);
                Get();
                break;
            }
            Get();
            return m_Token = { .Type = TokenType::String, .Value = std::move(value) };
        }
    }

    return m_Token = { .Type = TokenType::EndOfFile };
}

bool scc::ir::Parser::At(const TokenType type) const
{
    return m_Token.Type == type;
}

bool scc::ir::Parser::At(const std::vector<TokenType> &types) const
{
    for (auto &type : types)
        if (m_Token.Type == type)
            return true;

    return false;
}

bool scc::ir::Parser::At(const TokenType type, const std::string_view value) const
{
    return m_Token.Type == type && m_Token.Value == value;
}

bool scc::ir::Parser::At(const TokenType type, const std::vector<std::string_view> &values) const
{
    if (m_Token.Type != type)
        return false;

    for (auto &value : values)
        if (m_Token.Value == value)
            return true;

    return false;
}

bool scc::ir::Parser::Skip(const TokenType type)
{
    if (m_Token.Type == type)
    {
        Next();
        return true;
    }
    return false;
}

bool scc::ir::Parser::Skip(const TokenType type, const std::string_view value)
{
    if (m_Token.Type == type && m_Token.Value == value)
    {
        Next();
        return true;
    }
    return false;
}

scc::ir::Token scc::ir::Parser::Skip()
{
    auto token = m_Token;
    Next();
    return token;
}

scc::ir::Token scc::ir::Parser::Expect(const TokenType type)
{
    Assert(m_Token.Type == type, "unexpected token");
    return Skip();
}

scc::ir::Token scc::ir::Parser::Expect(const std::vector<TokenType> &types)
{
    for (const auto &type : types)
        if (m_Token.Type == type)
            return Skip();

    Error("unexpected token");
}

scc::ir::Token scc::ir::Parser::Expect(const TokenType type, const std::string_view value)
{
    Assert(m_Token.Type == type && m_Token.Value == value, "unexpected token");
    return Skip();
}

scc::ir::Token scc::ir::Parser::Expect(const TokenType type, const std::vector<std::string_view> &values)
{
    Assert(m_Token.Type == type, "unexpected token");

    for (const auto &value : values)
        if (m_Token.Value == value)
            return Skip();

    Error("unexpected token");
}

scc::ir::Module scc::ir::Parser::ParseModule(Context &context)
{
    Module module;
    Builder builder(context);

    while (m_Token.Type != TokenType::EndOfFile)
    {
        if (Skip(TokenType::EndOfLine))
            continue;

        if (Skip(TokenType::Identifier, "variable"))
        {
            Expect(TokenType::Other, "@");

            auto name = Expect(TokenType::Identifier).Value;

            Expect(TokenType::Other, "=");

            auto *type = ParseType(context);
            auto *value = ParseConstant(builder, type);

            module.CreateVariable(type, std::move(name), value);

            Expect(TokenType::EndOfLine);
            continue;
        }

        if (Skip(TokenType::Identifier, "function"))
        {
            auto *result = ParseType(builder.GetContext());

            Expect(TokenType::Other, "@");

            auto name = Expect(TokenType::Identifier).Value;

            auto variadic = false;

            std::vector<Type *> arguments;
            std::vector<std::string> labels;

            Expect(TokenType::Other, "(");

            while (!At(TokenType::Other, ")"))
            {
                if (Skip(TokenType::Other, "..."))
                {
                    variadic = true;
                    break;
                }

                arguments.push_back(ParseType(builder.GetContext()));

                auto &label = labels.emplace_back();

                if (Skip(TokenType::Other, "%"))
                    label = Expect(TokenType::Identifier).Value;

                if (!At(TokenType::Other, ")"))
                    Expect(TokenType::Other, ",");
            }

            Expect(TokenType::Other, ")");

            auto *type = context.GetFunctionType(result, std::move(arguments), variadic);
            auto *function = module.CreateFunction(type, std::move(name));

            for (size_t i = 0; i < labels.size(); ++i)
                if (!labels[i].empty())
                    function->GetArgument(i)->SetName(std::move(labels[i]));

            if (Skip(TokenType::Other, "{"))
            {
                Expect(TokenType::EndOfLine);

                while (!At(TokenType::Other, "}"))
                {
                    if (Skip(TokenType::Other, "."))
                    {
                        auto block_name = Expect(TokenType::Identifier).Value;
                        Expect(TokenType::Other, ":");
                        Expect(TokenType::EndOfLine);

                        auto *block = builder.GetOrCreateBlock(function, std::move(block_name));
                        builder.SetInsertBlock(block);
                    }

                    ParseInstruction(module, builder);
                    Expect(TokenType::EndOfLine);
                }

                Expect(TokenType::Other, "}");

                builder.ClearInsertBlock();
            }

            Expect(TokenType::EndOfLine);
            continue;
        }

        std::cout << m_Token.Type << ':' << m_Token.Value << std::endl;

        Next();
    }

    return module;
}

scc::ir::Type *scc::ir::Parser::ParseType(Context &context)
{
    Type *base;
    if (Skip(TokenType::Other, "["))
    {
        auto *element = ParseType(context);

        if (Skip(TokenType::Identifier, "x"))
        {
            const auto length = Skip().IntValue;

            Expect(TokenType::Other, "]");

            base = context.GetArrayType(element, length);
        }
        else
        {
            Expect(TokenType::Other, "]");

            base = context.GetPointerType(element);
        }
    }
    else if (Skip(TokenType::Other, "<"))
    {
        auto *element = ParseType(context);

        Expect(TokenType::Identifier, "x");

        const auto length = Skip().IntValue;

        Expect(TokenType::Other, ">");

        base = context.GetVectorType(element, length);
    }
    else if (Skip(TokenType::Other, "{"))
    {
        std::vector<Type *> elements;

        while (!At(TokenType::Other, "}"))
        {
            elements.push_back(ParseType(context));

            if (!At(TokenType::Other, "}"))
                Expect(TokenType::Other, ",");
        }

        Expect(TokenType::Other, "}");

        base = context.GetStructType(std::move(elements));
    }
    else
    {
        if (const auto symbol = Expect(TokenType::Identifier).Value; symbol == "void")
            base = context.GetVoidType();
        else if (symbol == "i1")
            base = context.GetInt1Type();
        else if (symbol == "i8")
            base = context.GetInt8Type();
        else if (symbol == "i16")
            base = context.GetInt16Type();
        else if (symbol == "i32")
            base = context.GetInt32Type();
        else if (symbol == "i64")
            base = context.GetInt64Type();
        else if (symbol == "f32")
            base = context.GetFloat32Type();
        else if (symbol == "f64")
            base = context.GetFloat64Type();
        else
            Error("unexpected token");
    }

    if (Skip(TokenType::Other, "("))
    {
        auto variadic = false;

        std::vector<Type *> arguments;

        while (!At(TokenType::Other, ")"))
        {
            if (Skip(TokenType::Other, "..."))
            {
                variadic = true;
                break;
            }

            arguments.push_back(ParseType(context));

            if (!At(TokenType::Other, ")"))
                Expect(TokenType::Other, ",");
        }

        Expect(TokenType::Other, ")");

        return context.GetFunctionType(base, std::move(arguments), variadic);
    }

    return base;
}

scc::ir::Constant *scc::ir::Parser::ParseConstant(Builder &builder, Type *type)
{
    if (At(TokenType::String))
    {
        const auto value = Skip().Value;

        Assert(type->GetKind() == Kind::Array, "invalid value for non-array type");

        const auto *array_type = dynamic_cast<ArrayType *>(type);
        auto *element_type = array_type->GetElement();

        Assert(element_type->GetKind() == Kind::Int, "invalid value for non-int array type");

        const auto *int_type = dynamic_cast<IntType *>(element_type);

        Assert(int_type->GetBitWidth() == 8, "invalid value for non-8-bit int array type");

        return builder.GetContext().GetArray(value);
    }

    if (At(TokenType::Integer))
    {
        const auto value = Skip().IntValue;

        Assert(type->GetKind() == Kind::Int, "invalid value for non-int type");

        auto *int_type = dynamic_cast<IntType *>(type);

        return builder.GetContext().GetInt(int_type, value);
    }

    Error("unexpected token");
}

scc::ir::Value *scc::ir::Parser::ParseValue(Module &module, Builder &builder, Type *type)
{
    if (Skip(TokenType::Other, "%"))
    {
        auto name = Expect(TokenType::Identifier).Value;
        auto *value = builder.GetInsertFunction()->FindValue(name);

        if (!value)
            value = builder.CreateEmpty(type, std::move(name));

        return value;
    }

    if (Skip(TokenType::Other, "@"))
    {
        const auto name = Expect(TokenType::Identifier).Value;

        return module.GetSymbol(name);
    }

    return ParseConstant(builder, type);
}

scc::ir::Instruction *scc::ir::Parser::ParseInstruction(Module &module, Builder &builder)
{
    if (At(TokenType::Identifier, "br"))
        return ParseBranchInstruction(module, builder);
    if (At(TokenType::Identifier, "ret"))
        return ParseReturnInstruction(module, builder);
    if (At(TokenType::Identifier, "store"))
        return ParseStoreInstruction(module, builder);

    std::string name;
    if (Skip(TokenType::Other, "%"))
    {
        name = Expect(TokenType::Identifier).Value;

        Expect(TokenType::Other, "=");
    }

    if (At(TokenType::Identifier, "load"))
        return ParseLoadInstruction(module, builder, std::move(name));

    if (At(TokenType::Identifier, "call"))
        return ParseCallInstruction(module, builder, std::move(name));

    if (At(TokenType::Identifier, "element"))
        return ParseElementInstruction(module, builder, std::move(name));

    if (At(TokenType::Identifier, "select"))
        return ParseSelectInstruction(module, builder, std::move(name));

    if (At(TokenType::Identifier, "alloc"))
        return ParseAllocInstruction(module, builder, std::move(name));

    if (At(TokenType::Identifier, "cast"))
        return ParseCastInstruction(module, builder, std::move(name));

    if (At(
        TokenType::Identifier,
        "slt",
        "ult",
        "sgt",
        "ugt",
        "sle",
        "ule",
        "sge",
        "uge",
        "equ",
        "neq"))
        return ParseComparatorInstruction(module, builder, std::move(name));

    if (At(
        TokenType::Identifier,
        "add",
        "sub",
        "mul",
        "sdiv",
        "udiv",
        "srem",
        "urem",
        "and",
        "or",
        "xor"))
        return ParseOperatorInstruction(module, builder, std::move(name));

    Error("unexpected token");
}

scc::ir::Instruction *scc::ir::Parser::ParseBranchInstruction(Module &module, Builder &builder)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "br");

    if (Skip(TokenType::Other, "."))
    {
        auto destination_name = Expect(TokenType::Identifier).Value;
        auto *destination = builder.GetOrCreateBlock(builder.GetInsertFunction(), std::move(destination_name));

        return builder.CreateBranch(destination);
    }

    auto *condition = ParseValue(module, builder, context.GetInt1Type());

    Expect(TokenType::Other, ",");

    Expect(TokenType::Other, ".");

    auto then_name = Expect(TokenType::Identifier).Value;
    auto *then = builder.GetOrCreateBlock(builder.GetInsertFunction(), std::move(then_name));

    Expect(TokenType::Other, ",");

    Expect(TokenType::Other, ".");

    auto else_name = Expect(TokenType::Identifier).Value;
    auto *else_ = builder.GetOrCreateBlock(builder.GetInsertFunction(), std::move(else_name));

    return builder.CreateBranch(condition, then, else_);
}

scc::ir::Instruction *scc::ir::Parser::ParseReturnInstruction(Module &module, Builder &builder)
{
    Expect(TokenType::Identifier, "ret");

    if (At(TokenType::EndOfLine))
        return builder.CreateRet();

    auto *value = ParseValue(module, builder, builder.GetInsertFunctionResult());

    return builder.CreateRet(value);
}

scc::ir::Instruction *scc::ir::Parser::ParseStoreInstruction(Module &module, Builder &builder)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "store");

    auto *pointer_type = ParseType(context);
    auto *pointer = ParseValue(module, builder, pointer_type);

    auto *value_type = pointer_type->GetElement();
    auto *value = ParseValue(module, builder, value_type);

    return builder.CreateStore(pointer, value);
}

scc::ir::Instruction *scc::ir::Parser::ParseLoadInstruction(Module &module, Builder &builder, std::string name)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "load");

    auto *pointer_type = ParseType(context);
    auto *pointer = ParseValue(module, builder, pointer_type);

    return builder.CreateLoad(pointer, std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseComparatorInstruction(Module &module, Builder &builder, std::string name)
{
    static const std::unordered_map<std::string_view, Comparator> map
    {
        { "slt", Comparator::SLT },
        { "ult", Comparator::ULT },
        { "sgt", Comparator::SGT },
        { "ugt", Comparator::UGT },
        { "sle", Comparator::SLE },
        { "ule", Comparator::ULE },
        { "sge", Comparator::SGE },
        { "uge", Comparator::UGE },
        { "equ", Comparator::EQU },
        { "neq", Comparator::NEQ },
    };

    auto &context = builder.GetContext();

    const auto comparator = Expect(
        TokenType::Identifier,
        "slt",
        "ult",
        "sgt",
        "ugt",
        "sle",
        "ule",
        "sge",
        "uge",
        "equ",
        "neq"
    ).Value;

    auto *type = ParseType(context);

    auto *lhs = ParseValue(module, builder, type);

    Expect(TokenType::Other, ",");

    auto *rhs = ParseValue(module, builder, type);

    const auto it = map.find(comparator);

    Assert(it != map.end(), "undefined comparator '{}'", comparator);

    return builder.CreateComparator(it->second, type, lhs, rhs, std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseOperatorInstruction(Module &module, Builder &builder, std::string name)
{
    static const std::unordered_map<std::string_view, Operator> map
    {
        { "add", Operator::Add },
        { "sub", Operator::Sub },
        { "mul", Operator::Mul },
        { "sdiv", Operator::SDiv },
        { "udiv", Operator::UDiv },
        { "srem", Operator::SRem },
        { "urem", Operator::URem },
        { "and", Operator::And },
        { "or", Operator::Or },
        { "xor", Operator::Xor },
    };

    auto &context = builder.GetContext();

    auto operator_ = Expect(
        TokenType::Identifier,
        "add",
        "sub",
        "mul",
        "sdiv",
        "udiv",
        "srem",
        "urem",
        "and",
        "or",
        "xor"
    ).Value;

    auto *type = ParseType(context);

    std::vector<Value *> operands;

    while (!At(TokenType::EndOfLine))
    {
        operands.push_back(ParseValue(module, builder, type));

        if (!At(TokenType::EndOfLine))
            Expect(TokenType::Other, ",");
    }

    const auto it = map.find(operator_);

    Assert(it != map.end(), "undefined operator '{}'", operator_);

    return builder.CreateOperator(it->second, type, std::move(operands), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseCallInstruction(Module &module, Builder &builder, std::string name)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "call");

    auto *type = ParseType(context);

    Assert(type->GetKind() == Kind::Function, "type {} is not a kind of function", type);

    auto *function_type = dynamic_cast<FunctionType *>(type);
    const auto argument_count = function_type->GetArgumentCount();

    auto *callee = ParseValue(module, builder, context.GetPointerType(function_type));

    std::vector<Value *> arguments;
    arguments.reserve(argument_count);

    while (!At(TokenType::EndOfLine))
    {
        Expect(TokenType::Other, ",");

        const auto index = arguments.size();

        Type *argument_type;
        if (index < argument_count)
            argument_type = function_type->GetArgument(index);
        else
            argument_type = ParseType(context);

        auto *argument = ParseValue(module, builder, argument_type);

        arguments.push_back(argument);
    }

    return builder.CreateCall(function_type, callee, std::move(arguments), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseElementInstruction(
    Module &module,
    Builder &builder,
    std::string name)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "element");

    auto *pointer_type = ParseType(context);
    auto *pointer = ParseValue(module, builder, pointer_type);

    Expect(TokenType::Other, ",");

    std::vector<Value *> indices;

    while (!At(TokenType::EndOfLine))
    {
        auto *index_type = ParseType(context);
        auto *index = ParseValue(module, builder, index_type);

        indices.push_back(index);

        if (!At(TokenType::EndOfLine))
            Expect(TokenType::Other, ",");
    }

    return builder.CreateElementPointer(pointer, std::move(indices), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseSelectInstruction(Module &module, Builder &builder, std::string name)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "select");

    auto *type = ParseType(context);

    std::vector<std::pair<Block *, Value *>> nodes;

    while (!At(TokenType::EndOfLine))
    {
        Expect(TokenType::Other, "[");

        Expect(TokenType::Other, ".");

        auto block_name = Expect(TokenType::Identifier).Value;
        auto *block = builder.GetOrCreateBlock(builder.GetInsertFunction(), std::move(block_name));

        Expect(TokenType::Other, ",");

        auto *value = ParseValue(module, builder, type);

        Expect(TokenType::Other, "]");

        nodes.emplace_back(block, value);

        if (!At(TokenType::EndOfLine))
            Expect(TokenType::Other, ",");
    }

    return builder.CreateSelect(type, std::move(nodes), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseAllocInstruction(Module &module, Builder &builder, std::string name)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "alloc");

    auto *type = ParseType(context);

    const auto count = Expect(TokenType::Integer).IntValue;

    return builder.CreateAlloc(type, count, std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseCastInstruction(Module &module, Builder &builder, std::string name)
{
    auto &context = builder.GetContext();

    Expect(TokenType::Identifier, "cast");

    auto *type = ParseType(context);

    Expect(TokenType::Other, ",");

    auto *value_type = ParseType(context);
    auto *value = ParseValue(module, builder, value_type);

    return builder.CreateCast(type, value, std::move(name));
}

std::ostream &scc::ir::operator<<(std::ostream &stream, const TokenType type)
{
    static std::unordered_map<TokenType, const char *> map
    {
        { TokenType::EndOfFile, "EndOfFile" },
        { TokenType::EndOfLine, "EndOfLine" },
        { TokenType::Identifier, "Identifier" },
        { TokenType::Integer, "Integer" },
        { TokenType::String, "String" },
        { TokenType::Other, "Other" },
    };

    return stream << map.at(type);
}

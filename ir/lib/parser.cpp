#include <scc/ir/argument.hpp>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/function.hpp>
#include <scc/ir/parser.hpp>

#include <iostream>
#include <istream>
#include <map>
#include <unordered_map>

scc::ir::Parser::Parser(std::istream &stream, Context &context, Module &module)
    : m_Stream(stream),
      m_Context(context),
      m_Module(module),
      m_Builder(context)
{
    m_Buffer = m_Stream.get();
    Next();
}

void scc::ir::Parser::Parse()
{
    while (m_Token.Type != TokenType::EndOfFile)
    {
        if (Skip(TokenType::EndOfLine))
            continue;

        if (Skip(TokenType::Identifier, "variable"))
        {
            Expect(TokenType::Other, "@");

            auto name = Expect(TokenType::Identifier).Value;

            Expect(TokenType::Other, "=");

            auto *type = ParseType();
            auto *value = ParseConstant(type);

            m_Module.CreateVariable(type, std::move(name), value);

            Expect(TokenType::EndOfLine);
            continue;
        }

        if (Skip(TokenType::Identifier, "function"))
        {
            auto *result = ParseType();

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

                arguments.push_back(ParseType());

                auto &label = labels.emplace_back();

                if (Skip(TokenType::Other, "%"))
                    label = Expect(TokenType::Identifier).Value;

                if (!At(TokenType::Other, ")"))
                    Expect(TokenType::Other, ",");
            }

            Expect(TokenType::Other, ")");

            auto *type = m_Context.GetFunctionType(result, std::move(arguments), variadic);
            auto *function = m_Module.CreateFunction(type, std::move(name));

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

                        auto *block = m_Builder.GetOrCreateBlock(function, std::move(block_name));
                        m_Builder.SetInsertBlock(block);
                    }

                    ParseInstruction();
                    Expect(TokenType::EndOfLine);
                }

                Expect(TokenType::Other, "}");

                m_Builder.ClearInsertBlock();
            }

            Expect(TokenType::EndOfLine);
            continue;
        }

        std::cout << m_Token.Type << ':' << m_Token.Value << std::endl;

        Next();
    }
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
                const auto int_value = std::stoull(value, {}, static_cast<int>(base));
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

                        m_Buffer = std::stoi(buffer, {}, 0x10);
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

scc::ir::Type *scc::ir::Parser::ParseType()
{
    Type *base;
    if (Skip(TokenType::Other, "["))
    {
        auto *element = ParseType();

        if (Skip(TokenType::Identifier, "x"))
        {
            const auto length = Skip().IntValue;

            Expect(TokenType::Other, "]");

            base = m_Context.GetArrayType(element, length);
        }
        else
        {
            Expect(TokenType::Other, "]");

            base = m_Context.GetPointerType(element);
        }
    }
    else if (Skip(TokenType::Other, "<"))
    {
        auto *element = ParseType();

        Expect(TokenType::Identifier, "x");

        const auto length = Skip().IntValue;

        Expect(TokenType::Other, ">");

        base = m_Context.GetVectorType(element, length);
    }
    else if (Skip(TokenType::Other, "{"))
    {
        std::vector<Type *> elements;

        while (!At(TokenType::Other, "}"))
        {
            elements.push_back(ParseType());

            if (!At(TokenType::Other, "}"))
                Expect(TokenType::Other, ",");
        }

        Expect(TokenType::Other, "}");

        base = m_Context.GetStructType(std::move(elements));
    }
    else
    {
        if (const auto symbol = Expect(TokenType::Identifier).Value; symbol == "void")
            base = m_Context.GetVoidType();
        else if (symbol == "i1")
            base = m_Context.GetInt1Type();
        else if (symbol == "i8")
            base = m_Context.GetInt8Type();
        else if (symbol == "i16")
            base = m_Context.GetInt16Type();
        else if (symbol == "i32")
            base = m_Context.GetInt32Type();
        else if (symbol == "i64")
            base = m_Context.GetInt64Type();
        else if (symbol == "f32")
            base = m_Context.GetFloat32Type();
        else if (symbol == "f64")
            base = m_Context.GetFloat64Type();
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

            arguments.push_back(ParseType());

            if (!At(TokenType::Other, ")"))
                Expect(TokenType::Other, ",");
        }

        Expect(TokenType::Other, ")");

        return m_Context.GetFunctionType(base, std::move(arguments), variadic);
    }

    return base;
}

scc::ir::Constant *scc::ir::Parser::ParseConstant(Type *type)
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

        return m_Builder.GetContext().GetArray(value);
    }

    if (At(TokenType::Integer))
    {
        const auto value = Skip().IntValue;

        switch (type->GetKind())
        {
        case Kind::Int:
            return m_Builder.GetContext().GetInt(dynamic_cast<IntType *>(type), value);
        case Kind::Float:
            return m_Builder.GetContext().GetFloat(
                dynamic_cast<FloatType *>(type),
                reinterpret_cast<const float64_t &>(value));

        default:
            Error("invalid value for type {}", type);
        }
    }

    Error("unexpected token");
}

scc::ir::Value *scc::ir::Parser::ParseValue(Type *type)
{
    if (Skip(TokenType::Other, "%"))
    {
        auto name = Expect(TokenType::Identifier).Value;
        auto *value = m_Builder.GetInsertFunction()->FindValue(name);

        if (!value)
            value = m_Builder.CreateEmpty(type, std::move(name));

        return value;
    }

    if (Skip(TokenType::Other, "@"))
    {
        auto name = Expect(TokenType::Identifier).Value;

        return m_Module.GetOrCreateSymbol(type, std::move(name));
    }

    return ParseConstant(type);
}

scc::ir::Instruction *scc::ir::Parser::ParseInstruction()
{
    if (At(TokenType::Identifier, "br"))
        return ParseBranchInstruction();
    if (At(TokenType::Identifier, "ret"))
        return ParseReturnInstruction();
    if (At(TokenType::Identifier, "store"))
        return ParseStoreInstruction();

    std::string name;
    if (Skip(TokenType::Other, "%"))
    {
        name = Expect(TokenType::Identifier).Value;

        Expect(TokenType::Other, "=");
    }

    if (At(TokenType::Identifier, "load"))
        return ParseLoadInstruction(std::move(name));

    if (At(TokenType::Identifier, "call"))
        return ParseCallInstruction(std::move(name));

    if (At(TokenType::Identifier, "element"))
        return ParseElementInstruction(std::move(name));

    if (At(TokenType::Identifier, "select"))
        return ParseSelectInstruction(std::move(name));

    if (At(TokenType::Identifier, "alloc"))
        return ParseAllocInstruction(std::move(name));

    if (At(TokenType::Identifier, "cast"))
        return ParseCastInstruction(std::move(name));

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
        return ParseComparatorInstruction(std::move(name));

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
        return ParseOperatorInstruction(std::move(name));

    Error("unexpected token");
}

scc::ir::Instruction *scc::ir::Parser::ParseBranchInstruction()
{
    auto &context = m_Builder.GetContext();

    Expect(TokenType::Identifier, "br");

    if (Skip(TokenType::Other, "."))
    {
        auto destination_name = Expect(TokenType::Identifier).Value;
        auto *destination = m_Builder.GetOrCreateBlock(m_Builder.GetInsertFunction(), std::move(destination_name));

        return m_Builder.CreateBranch(destination);
    }

    auto *condition = ParseValue(context.GetInt1Type());

    Expect(TokenType::Other, ",");

    Expect(TokenType::Other, ".");

    auto then_name = Expect(TokenType::Identifier).Value;
    auto *then = m_Builder.GetOrCreateBlock(m_Builder.GetInsertFunction(), std::move(then_name));

    Expect(TokenType::Other, ",");

    Expect(TokenType::Other, ".");

    auto else_name = Expect(TokenType::Identifier).Value;
    auto *else_ = m_Builder.GetOrCreateBlock(m_Builder.GetInsertFunction(), std::move(else_name));

    return m_Builder.CreateBranch(condition, then, else_);
}

scc::ir::Instruction *scc::ir::Parser::ParseReturnInstruction()
{
    Expect(TokenType::Identifier, "ret");

    if (At(TokenType::EndOfLine))
        return m_Builder.CreateRet();

    auto *value = ParseValue(m_Builder.GetInsertFunctionResult());

    return m_Builder.CreateRet(value);
}

scc::ir::Instruction *scc::ir::Parser::ParseStoreInstruction()
{
    Expect(TokenType::Identifier, "store");

    auto *pointer_type = ParseType();
    auto *pointer = ParseValue(pointer_type);

    auto *value_type = pointer_type->GetElement();
    auto *value = ParseValue(value_type);

    return m_Builder.CreateStore(pointer, value);
}

scc::ir::Instruction *scc::ir::Parser::ParseLoadInstruction(std::string name)
{
    Expect(TokenType::Identifier, "load");

    auto *pointer_type = ParseType();
    auto *pointer = ParseValue(pointer_type);

    return m_Builder.CreateLoad(pointer, std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseComparatorInstruction(std::string name)
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

    auto *type = ParseType();

    auto *lhs = ParseValue(type);

    Expect(TokenType::Other, ",");

    auto *rhs = ParseValue(type);

    const auto it = map.find(comparator);

    Assert(it != map.end(), "undefined comparator '{}'", comparator);

    return m_Builder.CreateComparator(it->second, type, lhs, rhs, std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseOperatorInstruction(std::string name)
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

    auto *type = ParseType();

    std::vector<Value *> operands;

    while (!At(TokenType::EndOfLine))
    {
        operands.push_back(ParseValue(type));

        if (!At(TokenType::EndOfLine))
            Expect(TokenType::Other, ",");
    }

    const auto it = map.find(operator_);

    Assert(it != map.end(), "undefined operator '{}'", operator_);

    return m_Builder.CreateOperator(it->second, type, std::move(operands), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseCallInstruction(std::string name)
{
    Expect(TokenType::Identifier, "call");

    auto *type = ParseType();

    Assert(type->GetKind() == Kind::Function, "type {} is not a kind of function", type);

    auto *function_type = dynamic_cast<FunctionType *>(type);
    const auto argument_count = function_type->GetArgumentCount();

    auto *callee = ParseValue(m_Context.GetPointerType(function_type));

    std::vector<Value *> arguments;
    arguments.reserve(argument_count);

    while (!At(TokenType::EndOfLine))
    {
        Expect(TokenType::Other, ",");

        auto *argument_type = ParseType();
        auto *argument = ParseValue(argument_type);

        arguments.push_back(argument);
    }

    return m_Builder.CreateCall(function_type, callee, std::move(arguments), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseElementInstruction(std::string name)
{
    Expect(TokenType::Identifier, "element");

    auto *pointer_type = ParseType();
    auto *pointer = ParseValue(pointer_type);

    Expect(TokenType::Other, ",");

    std::vector<Value *> indices;

    while (!At(TokenType::EndOfLine))
    {
        auto *index_type = ParseType();
        auto *index = ParseValue(index_type);

        indices.push_back(index);

        if (!At(TokenType::EndOfLine))
            Expect(TokenType::Other, ",");
    }

    return m_Builder.CreateElementPointer(pointer, std::move(indices), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseSelectInstruction(std::string name)
{
    Expect(TokenType::Identifier, "select");

    auto *type = ParseType();

    std::vector<std::pair<Block *, Value *>> nodes;

    while (!At(TokenType::EndOfLine))
    {
        Expect(TokenType::Other, "[");

        Expect(TokenType::Other, ".");

        auto block_name = Expect(TokenType::Identifier).Value;
        auto *block = m_Builder.GetOrCreateBlock(m_Builder.GetInsertFunction(), std::move(block_name));

        Expect(TokenType::Other, ",");

        auto *value = ParseValue(type);

        Expect(TokenType::Other, "]");

        nodes.emplace_back(block, value);

        if (!At(TokenType::EndOfLine))
            Expect(TokenType::Other, ",");
    }

    return m_Builder.CreateSelect(type, std::move(nodes), std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseAllocInstruction(std::string name)
{
    Expect(TokenType::Identifier, "alloc");

    auto *type = ParseType();

    const auto count = Expect(TokenType::Integer).IntValue;

    return m_Builder.CreateAlloc(type, count, std::move(name));
}

scc::ir::Instruction *scc::ir::Parser::ParseCastInstruction(std::string name)
{
    Expect(TokenType::Identifier, "cast");

    auto *type = ParseType();

    Expect(TokenType::Other, ",");

    auto *value_type = ParseType();
    auto *value = ParseValue(value_type);

    return m_Builder.CreateCast(type, value, std::move(name));
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

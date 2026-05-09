#include <scc/ir/argument.hpp>
#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/function.hpp>
#include <scc/ir/parser.hpp>

#include <scc/assert.hpp>
#include <scc/error.hpp>

#include <iostream>
#include <istream>
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
                return m_Token = { .Type = TokenType::EoL };

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

bool scc::ir::Parser::At(const TokenType type, const std::string_view value) const
{
    return m_Token.Type == type && m_Token.Value == value;
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

scc::ir::Token scc::ir::Parser::Expect(const TokenType type, const std::string_view value)
{
    Assert(m_Token.Type == type && m_Token.Value == value, "unexpected token");
    return Skip();
}

scc::ir::Token scc::ir::Parser::Expect(const std::vector<TokenType> &types)
{
    for (const auto &type : types)
        if (m_Token.Type == type)
            return Skip();
    Error("unexpected token");
}

scc::ir::Module scc::ir::Parser::ParseModule(Context &context)
{
    Module module;
    Builder builder(context);

    while (m_Token.Type != TokenType::EndOfFile)
    {
        if (Skip(TokenType::EoL))
            continue;

        if (Skip(TokenType::Identifier, "variable"))
        {
            Expect(TokenType::Other, "@");

            auto name = Expect(TokenType::Identifier).Value;

            Expect(TokenType::Other, "=");

            auto *value = ParseConstant(builder);
            auto *type = value->GetType();

            module.CreateVariable(type, std::move(name), value);

            Expect(TokenType::EoL);
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
                Expect(TokenType::EoL);

                while (!At(TokenType::Other, "}"))
                {
                    if (Skip(TokenType::Other, "."))
                    {
                        auto block_name = Expect(TokenType::Identifier).Value;
                        Expect(TokenType::Other, ":");
                        Expect(TokenType::EoL);

                        auto *block = builder.GetOrCreateBlock(function, std::move(block_name));
                        builder.SetInsertBlock(block);
                    }

                    ParseInstruction(module, builder);
                    Expect(TokenType::EoL);
                }

                Expect(TokenType::Other, "}");

                builder.ClearInsertBlock();
            }

            Expect(TokenType::EoL);
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

scc::ir::Constant *scc::ir::Parser::ParseConstant(Builder &builder)
{
    auto *type = ParseType(builder.GetContext());

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

scc::ir::Value *scc::ir::Parser::ParseValue(Module &module, Builder &builder)
{
    if (Skip(TokenType::Other, "%"))
    {
        const auto name = Expect(TokenType::Identifier).Value;
        auto *value = builder.GetInsertFunction()->FindValue(name);

        Assert(!!value, "value must not be null");

        return value;
    }

    if (Skip(TokenType::Other, "@"))
    {
        const auto name = Expect(TokenType::Identifier).Value;

        return module.GetSymbol(name);
    }

    return ParseConstant(builder);
}

scc::ir::Instruction *scc::ir::Parser::ParseInstruction(Module &module, Builder &builder)
{
    if (Skip(TokenType::Identifier, "store"))
    {
        auto *pointer = ParseValue(module, builder);

        Expect(TokenType::Other, ",");

        auto *value = ParseValue(module, builder);

        return builder.CreateStore(pointer, value);
    }

    if (Skip(TokenType::Identifier, "br"))
    {
        if (Skip(TokenType::Other, "."))
        {
            auto name = Expect(TokenType::Identifier).Value;
            auto *destination = builder.GetOrCreateBlock(builder.GetInsertFunction(), std::move(name));

            return builder.CreateBranch(destination);
        }

        auto *condition = ParseValue(module, builder);

        Expect(TokenType::Other, ",");
        Expect(TokenType::Other, ".");

        auto then_name = Expect(TokenType::Identifier).Value;

        Expect(TokenType::Other, ",");
        Expect(TokenType::Other, ".");

        auto else_name = Expect(TokenType::Identifier).Value;

        auto *then = builder.GetOrCreateBlock(builder.GetInsertFunction(), std::move(then_name));
        auto *else_ = builder.GetOrCreateBlock(builder.GetInsertFunction(), std::move(else_name));

        return builder.CreateBranch(condition, then, else_);
    }

    if (Skip(TokenType::Identifier, "ret"))
    {
        if (At(TokenType::EoL))
            return builder.CreateRet();

        auto *value = ParseValue(module, builder);

        return builder.CreateRet(value);
    }

    std::string name;
    if (Skip(TokenType::Other, "%"))
    {
        name = Expect(TokenType::Identifier).Value;

        Expect(TokenType::Other, "=");
    }

    if (Skip(TokenType::Identifier, "load"))
    {
        auto *pointer = ParseValue(module, builder);

        return builder.CreateLoad(pointer, std::move(name));
    }

    if (Skip(TokenType::Identifier, "cmp"))
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
            { "eq", Comparator::EQ },
            { "ne", Comparator::NE },
        };

        Expect(TokenType::Other, ".");

        const auto comparator = Expect(TokenType::Identifier).Value;
        auto *lhs = ParseValue(module, builder);

        Expect(TokenType::Other, ",");

        auto *rhs = ParseValue(module, builder);

        const auto it = map.find(comparator);

        Assert(it != map.end(), "undefined comparator '{}'", comparator);

        return builder.CreateComparator(it->second, lhs, rhs, std::move(name));
    }

    if (Skip(TokenType::Identifier, "call"))
    {
        auto *callee = ParseValue(module, builder);

        Expect(TokenType::Other, ",");

        std::vector<Value *> arguments;

        while (!At(TokenType::EoL))
        {
            arguments.push_back(ParseValue(module, builder));

            if (!At(TokenType::EoL))
                Expect(TokenType::Other, ",");
        }

        return builder.CreateCall(callee, std::move(arguments), std::move(name));
    }

    if (Skip(TokenType::Identifier, "add"))
    {
        std::vector<Value *> operands;
        while (!At(TokenType::EoL))
        {
            operands.push_back(ParseValue(module, builder));

            if (!At(TokenType::EoL))
                Expect(TokenType::Other, ",");
        }

        return builder.CreateAdd(std::move(operands), std::move(name));
    }

    auto *type = ParseType(builder.GetContext());

    if (Skip(TokenType::Identifier, "alloc"))
    {
        if (At(TokenType::EoL))
            return builder.CreateAlloc(type, std::move(name));

        const auto count = Expect(TokenType::Integer).IntValue;

        return builder.CreateAlloc(type, std::move(name), count);
    }

    if (Skip(TokenType::Identifier, "cast"))
    {
        auto *value = ParseValue(module, builder);

        return builder.CreateCast(type, value, std::move(name));
    }

    if (Skip(TokenType::Identifier, "offset"))
    {
        auto *base = ParseValue(module, builder);

        std::vector<Value *> offsets;

        Expect(TokenType::Other, ",");

        while (!At(TokenType::EoL))
        {
            offsets.push_back(ParseValue(module, builder));

            if (!At(TokenType::EoL))
                Expect(TokenType::Other, ",");
        }

        return builder.CreateOffset(type, base, std::move(offsets), std::move(name));
    }

    Error("unexpected token");
}

std::ostream &scc::ir::operator<<(std::ostream &stream, const TokenType type)
{
    static std::unordered_map<TokenType, const char *> map
    {
        { TokenType::EndOfFile, "EndOfFile" },
        { TokenType::Identifier, "Identifier" },
        { TokenType::Integer, "Integer" },
        { TokenType::String, "String" },
        { TokenType::Other, "Other" },
    };

    return stream << map.at(type);
}

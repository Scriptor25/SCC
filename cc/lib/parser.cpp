#include <scc/cc/context.hpp>
#include <scc/cc/parser.hpp>
#include <scc/cc/type.hpp>

#include <scc/assert.hpp>

#include <toolkit/string.hxx>

#include <optional>
#include <unordered_map>
#include <unordered_set>

scc::cc::Parser::Parser(std::istream &stream, Context &context, Module &module)
    : m_Stream(stream),
      m_Context(context),
      m_Module(module)
{
    m_Buffer = m_Stream.get();
    m_Token = Next();
}

void scc::cc::Parser::Parse()
{
    while (m_Token.Type != TokenType::None)
    {
        const auto node = ParseNode();
        node->Generate();
    }
}

std::unique_ptr<scc::cc::Node> scc::cc::Parser::ParseNode()
{
    if (At(TokenType::Identifier, "typedef"))
        return ParseTypeDefNode();

    auto *type = ParseType();
    auto name = Expect(TokenType::Identifier).Value;

    if (At(TokenType::Other, "("))
        return ParseFunctionNode(type, std::move(name));

    return ParseVariableNode(type, std::move(name));
}

std::unique_ptr<scc::cc::FunctionNode> scc::cc::Parser::ParseFunctionNode(Type *result, std::string name)
{
    Expect(TokenType::Other, "(");

    std::vector<FunctionArgument> arguments;
    while (!At(TokenType::Other, ")") && !At(TokenType::None))
    {
        if (!arguments.empty())
            Expect(TokenType::Other, ",");

        auto *argument_type = ParseType();

        std::optional<std::string> argument_name;
        if (At(TokenType::Identifier))
            argument_name = Skip().Value;

        arguments.push_back(
            {
                .Ty = argument_type,
                .Name = std::move(argument_name),
            });
    }

    Expect(TokenType::Other, ")");

    if (Skip(TokenType::Other, ";"))
        return std::make_unique<FunctionNode>(result, std::move(name), std::move(arguments));

    auto content = ParseStatementNode();

    return std::make_unique<FunctionNode>(result, std::move(name), std::move(arguments), std::move(content));
}

std::unique_ptr<scc::cc::VariableNode> scc::cc::Parser::ParseVariableNode(Type *type, std::string name)
{
    if (Skip(TokenType::Other, ";"))
        return std::make_unique<VariableNode>(type, std::move(name));

    auto value = ParseExpressionNode();

    Expect(TokenType::Other, ";");

    return std::make_unique<VariableNode>(type, std::move(name), std::move(value));
}

std::unique_ptr<scc::cc::TypeDefNode> scc::cc::Parser::ParseTypeDefNode()
{
    Expect(TokenType::Identifier, "typedef");

    auto *type = ParseType();

    auto name = Expect(TokenType::Identifier).Value;

    Expect(TokenType::Other, ";");

    return std::make_unique<TypeDefNode>(type, std::move(name));
}

std::unique_ptr<scc::cc::StatementNode> scc::cc::Parser::ParseStatementNode()
{
    Error("TODO");
}

std::unique_ptr<scc::cc::ExpressionNode> scc::cc::Parser::ParseExpressionNode()
{
    Error("TODO");
}

scc::cc::Type *scc::cc::Parser::ParseType()
{
    // TODO: const

    auto *aggregate = ParseBaseType();

    for (;;)
    {
        if (Skip(TokenType::Other, "["))
        {
            if (Skip(TokenType::Other, "]"))
                aggregate = m_Context.GetArrayType(aggregate);
            else
            {
                const auto count = Expect(TokenType::Integer).IntegerValue;
                aggregate = m_Context.GetArrayType(aggregate, count);
                Expect(TokenType::Other, "]");
            }

            continue;
        }

        if (Skip(TokenType::Operator, "*"))
        {
            aggregate = m_Context.GetPointerType(aggregate);
            continue;
        }

        break;
    }

    return aggregate;
}

scc::cc::Type *scc::cc::Parser::ParseBaseType()
{
    if (At(TokenType::Identifier, "struct"))
        return ParseStructType();

    if (At(TokenType::Identifier, "union"))
        return ParseUnionType();

    if (At(TokenType::Identifier, "enum"))
        return ParseEnumType();

    if (At(TokenType::Identifier))
        if (auto *type = m_Context.GetNamedType(m_Token.Value))
            return type;

    if (At(TokenType::Identifier, "void"))
        return m_Context.GetVoidType();

    if (At(TokenType::Identifier, "bool"))
        return m_Context.GetBooleanType();

    enum class Base { None, Char, Int, Float, Double } base = Base::None;
    enum class Signedness { None, Signed, Unsigned } signedness = Signedness::None;
    unsigned short_count{}, long_count{};

    for (;;)
    {
        if (Skip(TokenType::Identifier, "char"))
        {
            base = Base::Char;
            continue;
        }

        if (Skip(TokenType::Identifier, "int"))
        {
            base = Base::Int;
            continue;
        }

        if (Skip(TokenType::Identifier, "float"))
        {
            base = Base::Float;
            continue;
        }

        if (Skip(TokenType::Identifier, "double"))
        {
            base = Base::Double;
            continue;
        }

        if (Skip(TokenType::Identifier, "signed"))
        {
            signedness = Signedness::Signed;
            continue;
        }

        if (Skip(TokenType::Identifier, "unsigned"))
        {
            signedness = Signedness::Unsigned;
            continue;
        }

        if (Skip(TokenType::Identifier, "short"))
        {
            ++short_count;
            continue;
        }

        if (Skip(TokenType::Identifier, "long"))
        {
            ++long_count;
            continue;
        }

        break;
    }

    if (short_count && long_count)
        Error("TODO");
    if (short_count > 1)
        Error("TODO");
    if (long_count > 2)
        Error("TODO");

    switch (base)
    {
    case Base::None:
        switch (signedness)
        {
        case Signedness::None:
            if (short_count)
                return m_Context.GetIntegerType(IntegerKind::SignedShort);
            switch (long_count)
            {
            case 1:
                return m_Context.GetIntegerType(IntegerKind::SignedLong);
            case 2:
                return m_Context.GetIntegerType(IntegerKind::SignedLongLong);
            default:
                break;
            }
            break;

        case Signedness::Signed:
            if (short_count)
                return m_Context.GetIntegerType(IntegerKind::SignedShort);
            switch (long_count)
            {
            case 1:
                return m_Context.GetIntegerType(IntegerKind::SignedLong);
            case 2:
                return m_Context.GetIntegerType(IntegerKind::SignedLongLong);
            default:
                break;
            }
            return m_Context.GetIntegerType(IntegerKind::SignedInt);

        case Signedness::Unsigned:
            if (short_count)
                return m_Context.GetIntegerType(IntegerKind::UnsignedShort);
            switch (long_count)
            {
            case 1:
                return m_Context.GetIntegerType(IntegerKind::UnsignedLong);
            case 2:
                return m_Context.GetIntegerType(IntegerKind::UnsignedLongLong);
            default:
                break;
            }
            return m_Context.GetIntegerType(IntegerKind::UnsignedInt);
        }
        break;

    case Base::Char:
        switch (signedness)
        {
        case Signedness::None:
            if (!short_count && !long_count)
                return m_Context.GetIntegerType(IntegerKind::Char);
            break;

        case Signedness::Signed:
            if (!short_count && !long_count)
                return m_Context.GetIntegerType(IntegerKind::SignedChar);
            break;

        case Signedness::Unsigned:
            if (!short_count && !long_count)
                return m_Context.GetIntegerType(IntegerKind::UnsignedChar);
            break;
        }
        break;

    case Base::Int:
        switch (signedness)
        {
        case Signedness::None:
        case Signedness::Signed:
            if (short_count)
                return m_Context.GetIntegerType(IntegerKind::SignedShort);
            switch (long_count)
            {
            case 1:
                return m_Context.GetIntegerType(IntegerKind::SignedLong);
            case 2:
                return m_Context.GetIntegerType(IntegerKind::SignedLongLong);
            default:
                break;
            }
            return m_Context.GetIntegerType(IntegerKind::SignedInt);

        case Signedness::Unsigned:
            if (short_count)
                return m_Context.GetIntegerType(IntegerKind::UnsignedShort);
            switch (long_count)
            {
            case 1:
                return m_Context.GetIntegerType(IntegerKind::UnsignedLong);
            case 2:
                return m_Context.GetIntegerType(IntegerKind::UnsignedLongLong);
            default:
                break;
            }
            return m_Context.GetIntegerType(IntegerKind::UnsignedInt);
        }
        break;

    case Base::Float:
        if (signedness == Signedness::None && !short_count && !long_count)
            return m_Context.GetFloatingPointType(FloatingPointKind::Float);
        break;

    case Base::Double:
        if (signedness == Signedness::None && !short_count)
        {
            switch (long_count)
            {
            case 0:
                return m_Context.GetFloatingPointType(FloatingPointKind::Double);
            case 1:
                return m_Context.GetFloatingPointType(FloatingPointKind::LongDouble);
            default:
                break;
            }
        }
        break;
    }

    Error("TODO");
}

scc::cc::Type *scc::cc::Parser::ParseStructType()
{
    Expect(TokenType::Identifier, "struct");

    std::optional<std::string> name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (name && !At(TokenType::Other, "{"))
        return m_Context.GetStructType(std::move(*name));

    Expect(TokenType::Other, "{");

    std::vector<StructElement> elements;
    while (!At(TokenType::Other, "}") && !At(TokenType::None))
    {
        auto *element_type = ParseType();

        std::optional<std::string> element_name;
        if (At(TokenType::Identifier))
            element_name = Skip().Value;

        std::optional<uint8_t> element_bits;
        if (Skip(TokenType::Other, ":"))
            element_bits = Expect(TokenType::Integer).IntegerValue;

        Expect(TokenType::Other, ";");

        elements.push_back(
            {
                .Ty = element_type,
                .Name = std::move(element_name),
                .Bits = element_bits,
            });
    }

    Expect(TokenType::Other, "}");

    StructType *type;
    if (name)
    {
        type = m_Context.GetStructType(std::move(*name));

        type->Elements = std::move(elements);
    }
    else
        type = m_Context.GetStructType(std::move(elements));

    return type;
}

scc::cc::Type *scc::cc::Parser::ParseUnionType()
{
    Expect(TokenType::Identifier, "union");

    std::optional<std::string> name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (name && !At(TokenType::Other, "{"))
        return m_Context.GetUnionType(std::move(*name));

    Expect(TokenType::Other, "{");

    std::vector<UnionElement> elements;
    while (!At(TokenType::Other, "}") && !At(TokenType::None))
    {
        auto *element_type = ParseType();

        std::optional<std::string> element_name;
        if (At(TokenType::Identifier))
            element_name = Skip().Value;

        Expect(TokenType::Other, ";");

        elements.push_back(
            {
                .Ty = element_type,
                .Name = std::move(element_name),
            });
    }

    Expect(TokenType::Other, "}");

    UnionType *type;

    if (name)
    {
        type = m_Context.GetUnionType(std::move(*name));

        type->Elements = std::move(elements);
    }
    else
        type = m_Context.GetUnionType(std::move(elements));

    return type;
}

scc::cc::Type *scc::cc::Parser::ParseEnumType()
{
    Expect(TokenType::Identifier, "enum");

    std::optional<std::string> name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (name && !At(TokenType::Other, "{"))
        return m_Context.GetEnumType(std::move(*name));

    Expect(TokenType::Other, "{");

    std::vector<EnumElement> elements;
    while (!At(TokenType::Other, "}") && !At(TokenType::None))
    {
        auto element_name = Expect(TokenType::Identifier).Value;

        std::optional<int64_t> element_value;
        if (Skip(TokenType::Operator, "="))
            element_value = ParseExpressionNode()->EvaluateConstantInteger();

        if (!At(TokenType::Other, "}"))
            Expect(TokenType::Other, ",");

        elements.push_back(
            {
                .Name = std::move(element_name),
                .Value = element_value,
            });
    }

    Expect(TokenType::Other, "}");

    EnumType *type;

    if (name)
    {
        type = m_Context.GetEnumType(std::move(*name));

        type->Elements = std::move(elements);
    }
    else
        type = m_Context.GetEnumType(std::move(elements));

    return type;
}

bool scc::cc::Parser::At(const TokenType type) const
{
    return m_Token.Type == type;
}

bool scc::cc::Parser::At(const TokenType type, const std::string_view value) const
{
    return m_Token.Type == type && m_Token.Value == value;
}

scc::cc::Token scc::cc::Parser::Expect(TokenType type)
{
    if (At(type))
        return Skip();
    Error("TODO");
}

void scc::cc::Parser::Expect(TokenType type, std::string_view value)
{
    if (Skip(type, value))
        return;
    Error("TODO");
}

bool scc::cc::Parser::Skip(TokenType type)
{
    if (At(type))
    {
        m_Token = Next();
        return true;
    }
    return false;
}

bool scc::cc::Parser::Skip(TokenType type, std::string_view value)
{
    if (At(type, value))
    {
        m_Token = Next();
        return true;
    }
    return false;
}

scc::cc::Token scc::cc::Parser::Skip()
{
    auto token = std::move(m_Token);
    m_Token = Next();
    return token;
}

static bool isdigit(int c, int base)
{
    switch (base)
    {
    case 2:
        return '0' <= c && c <= '1';
    case 8:
        return '0' <= c && c <= '7';
    case 10:
        return '0' <= c && c <= '9';
    case 16:
        return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
    default:
        return false;
    }
}

scc::cc::Token scc::cc::Parser::Next()
{
    enum class State
    {
        None, LineComment, BlockComment, Operator, Identifier, Number, Char, String
    } state = State::None;

    std::string raw, value;

    int base{};
    bool floating_point{};

    auto push = [this, &raw, &value](const bool val = false)
    {
        raw += static_cast<char>(m_Buffer);
        if (val)
            value += static_cast<char>(m_Buffer);
        return m_Stream.get();
    };

    static const std::unordered_map<std::string, std::unordered_set<int>> operators
    {
        { "+", { '=', '+' } },
        { "-", { '=', '-' } },
        { "*", { '=' } },
        { "/", { '=' } },
        { "%", { '=' } },
        { "&", { '=', '&' } },
        { "&&", { '=' } },
        { "|", { '=', '|' } },
        { "||", { '=' } },
        { "^", { '=', '^' } },
        { "^^", { '=' } },
        { "!", { '=' } },
        { "=", { '=' } },
        { "<", { '=', '<' } },
        { "<<", { '=' } },
        { ">", { '=', '>' } },
        { ">>", { '=' } },
    };

    while (m_Buffer >= 0)
    {
        switch (state)
        {
        case State::None:
            switch (m_Buffer)
            {
            case '/':
                // either line comment, block comment or operator
                m_Buffer = push();
                switch (m_Buffer)
                {
                case '/':
                    // line comment
                    m_Buffer = push();
                    state = State::LineComment;
                    break;
                case '*':
                    // block comment
                    m_Buffer = push();
                    state = State::BlockComment;
                    break;
                default:
                    // operator
                    value = "/";
                    state = State::Operator;
                    break;
                }
                break;

            case '+':
            case '-':
            case '*':
            case '%':
            case '&':
            case '|':
            case '^':
            case '!':
            case '=':
            case '<':
            case '>':
                m_Buffer = push(true);
                state = State::Operator;
                break;

            case '\'':
                m_Buffer = push();
                state = State::Char;
                break;

            case '"':
                m_Buffer = push();
                state = State::String;
                break;

            case '0':
                // 0bBBB..., 0xXXX..., 0.DDD..., 0OOO...
                m_Buffer = push();
                switch (m_Buffer)
                {
                case 'b':
                case 'B':
                    m_Buffer = push();
                    base = 2;
                    state = State::Number;
                    break;

                case 'x':
                case 'X':
                    m_Buffer = push();
                    base = 16;
                    state = State::Number;
                    break;

                case '.':
                    m_Buffer = push();
                    value = "0.";
                    base = 10;
                    floating_point = true;
                    state = State::Number;
                    break;

                default:
                    value = "0";
                    base = 8;
                    state = State::Number;
                    break;
                }
                break;

            default:
                if (std::isspace(m_Buffer))
                {
                    m_Buffer = push();
                    break;
                }

                if (std::isalpha(m_Buffer) || m_Buffer == '_')
                {
                    state = State::Identifier;
                    break;
                }

                if (std::isdigit(m_Buffer))
                {
                    base = 10;
                    state = State::Number;
                    break;
                }

                m_Buffer = push(true);
                return { .Type = TokenType::Other, .Raw = std::move(raw), .Value = std::move(value) };
            }
            break;

        case State::LineComment:
            if (m_Buffer == '\n')
            {
                state = State::None;
                break;
            }

            m_Buffer = push();
            break;

        case State::BlockComment:
            if (m_Buffer == '*')
            {
                m_Buffer = push();

                if (m_Buffer == '/')
                {
                    m_Buffer = push();
                    state = State::None;
                    break;
                }

                break;
            }

            m_Buffer = push();
            break;

        case State::Operator:
            if (auto it = operators.find(value); it != operators.end())
                if (it->second.contains(m_Buffer))
                {
                    m_Buffer = push(true);
                    break;
                }

            return { .Type = TokenType::Operator, .Raw = std::move(raw), .Value = std::move(value) };

        case State::Identifier:
            if (std::isalnum(m_Buffer) || m_Buffer == '_')
            {
                m_Buffer = push(true);
                break;
            }

            return { .Type = TokenType::Identifier, .Raw = std::move(raw), .Value = std::move(value) };

        case State::Number:
            if (isdigit(m_Buffer, base))
            {
                m_Buffer = push(true);
                break;
            }

            if (base == 10 && !floating_point && m_Buffer == '.')
            {
                m_Buffer = push(true);
                floating_point = true;
                break;
            }

            if (floating_point)
            {
                long double val;
                if (auto res = toolkit::parse_string<long double>(value) >> val; !res)
                    Error("TODO");

                return { .Type = TokenType::FloatingPoint, .Raw = std::move(raw), .FloatingPointValue = val };
            }

            {
                uint64_t val;
                if (auto res = toolkit::parse_string<uint64_t>(value, base) >> val; !res)
                    Error("TODO");

                return { .Type = TokenType::Integer, .Raw = std::move(raw), .IntegerValue = val };
            }

        case State::Char:
            if (m_Buffer != '\'')
            {
                // TODO: escape characters

                m_Buffer = push(true);
                break;
            }

            m_Buffer = push();

            if (value.size() != 1)
                Error("TODO");

            return {
                .Type = TokenType::Integer,
                .Raw = std::move(raw),
                .IntegerValue = static_cast<uint64_t>(value.front()),
            };

        case State::String:
            if (m_Buffer != '"')
            {
                // TODO: escape characters

                m_Buffer = push(true);
                break;
            }

            m_Buffer = push();
            return { .Type = TokenType::String, .Raw = std::move(raw), .Value = std::move(value) };
        }
    }

    return { .Type = TokenType::None, .Raw = std::move(raw) };
}

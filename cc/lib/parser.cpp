#include <scc/cc/context.hpp>
#include <scc/cc/parser.hpp>
#include <scc/cc/type.hpp>

#include <scc/assert.hpp>

#include <toolkit/string.hxx>

#include <iostream>
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
        NodePtr node;
        if (auto res = ParseNode() >> node; !res)
        {
            std::cerr << res.error() << std::endl;
            break;
        }

        node->Generate();
    }
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseNode()
{
    if (At(TokenType::Identifier, "typedef"))
        return ParseTypeDefNode();

    Type *type;
    if (auto res = ParseType() >> type; !res)
        return res;

    std::string name;
    if (auto res = Expect(TokenType::Identifier).extract(&Token::Value) >> name; !res)
        return res;

    if (At(TokenType::Other, "("))
        return ParseFunctionNode(type, std::move(name));

    return ParseVariableNode(type, std::move(name));
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseFunctionNode(Type *result, std::string name)
{
    if (auto res = Expect(TokenType::Other, "("); !res)
        return res;

    std::vector<FunctionArgument> arguments;
    while (!At(TokenType::Other, ")") && !At(TokenType::None))
    {
        if (!arguments.empty())
            if (auto res = Expect(TokenType::Other, ","); !res)
                return res;

        Type *argument_type;
        if (auto res = ParseType() >> argument_type; !res)
            return res;

        std::optional<std::string> argument_name;
        if (At(TokenType::Identifier))
            argument_name = Skip().Value;

        arguments.push_back(
            {
                .Ty = argument_type,
                .Name = std::move(argument_name),
            });
    }

    if (auto res = Expect(TokenType::Other, ")"); !res)
        return res;

    if (Skip(TokenType::Other, ";"))
        return { std::make_unique<FunctionNode>(result, std::move(name), std::move(arguments)) };

    StatementNodePtr content;
    if (auto res = ParseStatementNode() >> content; !res)
        return res;

    return { std::make_unique<FunctionNode>(result, std::move(name), std::move(arguments), std::move(content)) };
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseVariableNode(Type *type, std::string name)
{
    if (Skip(TokenType::Other, ";"))
        return { std::make_unique<VariableNode>(type, std::move(name)) };

    ExpressionNodePtr value;
    if (auto res = ParseExpressionNode() >> value; !res)
        return res;

    if (auto res = Expect(TokenType::Other, ";"); !res)
        return res;

    return { std::make_unique<VariableNode>(type, std::move(name), std::move(value)) };
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseTypeDefNode()
{
    if (auto res = Expect(TokenType::Identifier, "typedef"); !res)
        return res;

    Type *type;
    if (auto res = ParseType() >> type; !res)
        return res;

    std::string name;
    if (auto res = Expect(TokenType::Identifier).extract(&Token::Value) >> name; !res)
        return res;

    if (auto res = Expect(TokenType::Other, ";"); !res)
        return res;

    return { std::make_unique<TypeDefNode>(type, std::move(name)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseStatementNode()
{
    if (At(TokenType::Other, "{"))
        return ParseSequenceStatementNode();

    if (CouldBeType())
        return ParseVariableStatementNode();

    ExpressionNodePtr node;
    if (auto res = ParseExpressionNode() >> node; !res)
        return res;

    if (auto res = Expect(TokenType::Other, ";"); !res)
        return res;

    return toolkit::result<ExpressionNodePtr>(std::move(node));
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseSequenceStatementNode()
{
    if (auto res = Expect(TokenType::Other, "{"); !res)
        return res;

    std::vector<StatementNodePtr> nodes;

    while (!At(TokenType::Other, "}") && !At(TokenType::None))
    {
        StatementNodePtr node;
        if (auto res = ParseStatementNode() >> node; !res)
            return res;

        nodes.push_back(std::move(node));
    }

    if (auto res = Expect(TokenType::Other, "}"); !res)
        return res;

    return { std::make_unique<SequenceStatementNode>(std::move(nodes)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseVariableStatementNode()
{
    Type *type;
    if (auto res = ParseType() >> type; !res)
        return res;

    std::vector<std::pair<std::string, ExpressionNodePtr>> elements;
    do
    {
        std::string name;
        if (auto res = Expect(TokenType::Identifier).extract(&Token::Value) >> name; !res)
            return res;

        ExpressionNodePtr value;
        if (Skip(TokenType::Operator, "="))
            if (auto res = ParseExpressionNode() >> value; !res)
                return res;

        elements.emplace_back(std::move(name), std::move(value));
    }
    while (Skip(TokenType::Other, ","));

    if (auto res = Expect(TokenType::Other, ";"); !res)
        return res;

    return { std::make_unique<VariableStatementNode>(type, std::move(elements)) };
}

toolkit::result<scc::cc::ExpressionNodePtr> scc::cc::Parser::ParseExpressionNode()
{
    return ParseBinaryExpressionNode();
}

toolkit::result<scc::cc::ExpressionNodePtr> scc::cc::Parser::ParseBinaryExpressionNode()
{
    ExpressionNodePtr operand;
    if (auto res = ParseOperandExpressionNode(); !res)
        return res;

    return ParseBinaryExpressionNode(std::move(operand), 0);
}

toolkit::result<scc::cc::ExpressionNodePtr> scc::cc::Parser::ParseBinaryExpressionNode(
    ExpressionNodePtr left,
    size_t min_precedence)
{
    struct OperatorDefinition
    {
        bool RightAssociate;
        size_t Precedence;
        BinaryOperator Operator = BinaryOperator::Special;
    };

    static const std::unordered_map<std::string_view, OperatorDefinition> precedence
    {
        { "*", { false, 3, BinaryOperator::Multiply } },
        { "/", { false, 3, BinaryOperator::Divide } },
        { "%", { false, 3, BinaryOperator::Remainder } },
        { "+", { false, 4, BinaryOperator::Add } },
        { "-", { false, 4, BinaryOperator::Subtract } },
        { "<<", { false, 5, BinaryOperator::ShiftLeft } },
        { ">>", { false, 5, BinaryOperator::ShiftRight } },
        { "<", { false, 6, BinaryOperator::CompareLessThan } },
        { "<=", { false, 6, BinaryOperator::CompareLessThanEqual } },
        { ">", { false, 6, BinaryOperator::CompareGreaterThen } },
        { ">=", { false, 6, BinaryOperator::CompareGreaterThenEqual } },
        { "==", { false, 7, BinaryOperator::CompareEqual } },
        { "!=", { false, 7, BinaryOperator::CompareNotEqual } },
        { "&", { false, 8, BinaryOperator::And } },
        { "^", { false, 9, BinaryOperator::XOr } },
        { "|", { false, 10, BinaryOperator::Or } },
        { "&&", { false, 11, BinaryOperator::LogicalAnd } },
        { "||", { false, 12, BinaryOperator::LogicalOr } },
        { "?", { true, 13 } },
        { "=", { true, 14, BinaryOperator::Assign } },
        { "+=", { true, 14, BinaryOperator::AddAssign } },
        { "-=", { true, 14, BinaryOperator::SubtractAssign } },
        { "*=", { true, 14, BinaryOperator::MultiplyAssign } },
        { "/=", { true, 14, BinaryOperator::DivideAssign } },
        { "%=", { true, 14, BinaryOperator::RemainderAssign } },
        { "<<=", { true, 14, BinaryOperator::ShiftLeftAssign } },
        { ">>=", { true, 14, BinaryOperator::ShiftRightAssign } },
        { "&=", { true, 14, BinaryOperator::AndAssign } },
        { "^=", { true, 14, BinaryOperator::XOrAssign } },
        { "|=", { true, 14, BinaryOperator::OrAssign } },
        { ",", { false, 15 } },
    };

    while (true)
    {
        const auto before_it = precedence.find(m_Token.Value);
        if (before_it == precedence.end() || before_it->second.Precedence < min_precedence)
            break;

        const auto before = before_it->second;

        auto o = Skip().Value;

        ExpressionNodePtr right;
        if (auto res = ParseOperandExpressionNode(); !res)
            return res;

        while (true)
        {
            const auto after_it = precedence.find(m_Token.Value);
            if (after_it == precedence.end() ||
                (!after_it->second.RightAssociate && after_it->second.Precedence < before.Precedence) ||
                (after_it->second.RightAssociate && after_it->second.Precedence != before.Precedence))
                break;

            const auto after = after_it->second;

            if (auto res =
                    ParseBinaryExpressionNode(
                        std::move(right),
                        before.Precedence + (after.Precedence > before.Precedence ? 1 : 0)) >> right; !res)
                return res;
        }

        if (o == "?")
        {
            if (auto res = Expect(TokenType::Other, ":"); !res)
                return res;

            ExpressionNodePtr else_;
            if (auto res = ParseOperandExpressionNode() >> else_; !res)
                return res;

            left = std::make_unique<TernaryExpressionNode>(std::move(left), std::move(right), std::move(else_));
        }
        else
        {
            left = std::make_unique<BinaryExpressionNode>(before.Operator, std::move(left), std::move(right));
        }
    }

    return left;
}

toolkit::result<scc::cc::ExpressionNodePtr> scc::cc::Parser::ParseOperandExpressionNode()
{
    ExpressionNodePtr operand;
    if (auto res = ParsePrimaryExpressionNode() >> operand; !res)
        return res;

    while (true)
    {
        if (Skip(TokenType::Operator, "++"))
        {
            operand = std::make_unique<UnaryExpressionNode>(UnaryOperator::SuffixIncrement, std::move(operand));
            continue;
        }

        if (Skip(TokenType::Operator, "--"))
        {
            operand = std::make_unique<UnaryExpressionNode>(UnaryOperator::SuffixDecrement, std::move(operand));
            continue;
        }

        if (Skip(TokenType::Other, "("))
        {
            std::vector<ExpressionNodePtr> arguments;

            // TODO: parse arguments

            operand = std::make_unique<CallExpressionNode>(std::move(operand), std::move(arguments));
            continue;
        }

        if (Skip(TokenType::Other, "["))
        {
            ExpressionNodePtr index;
            if (auto res = ParseExpressionNode() >> index; !res)
                return res;

            operand = std::make_unique<SubscriptExpressionNode>(std::move(operand), std::move(index));
            continue;
        }

        if (Skip(TokenType::Other, "."))
        {
            Token token;
            if (auto res = Expect(TokenType::Identifier) >> token; !res)
                return res;

            auto name = std::move(token).Value;

            operand = std::make_unique<MemberExpressionNode>(std::move(operand), std::move(name), false);
            continue;
        }

        if (Skip(TokenType::Other, "->"))
        {
            Token token;
            if (auto res = Expect(TokenType::Identifier) >> token; !res)
                return res;

            auto name = std::move(token).Value;

            operand = std::make_unique<MemberExpressionNode>(std::move(operand), std::move(name), true);
            continue;
        }

        return operand;
    }
}

toolkit::result<scc::cc::ExpressionNodePtr> scc::cc::Parser::ParsePrimaryExpressionNode()
{
    if (At(TokenType::Operator))
    {
        static const std::unordered_map<std::string_view, UnaryOperator> operators
        {
            { "+", UnaryOperator::Positive },
            { "-", UnaryOperator::Negative },
            { "~", UnaryOperator::Not },
            { "!", UnaryOperator::LogicalNot },
            { "*", UnaryOperator::Dereference },
            { "&", UnaryOperator::Reference },
            { "++", UnaryOperator::PrefixIncrement },
            { "--", UnaryOperator::PrefixDecrement },
        };

        if (const auto it = operators.find(m_Token.Value); it != operators.end())
        {
            ExpressionNodePtr operand;
            if (auto res = ParseOperandExpressionNode() >> operand; !res)
                return res;

            return { std::make_unique<UnaryExpressionNode>(it->second, std::move(operand)) };
        }
    }

    if (Skip(TokenType::Identifier, "sizeof"))
    {
        if (auto res = Expect(TokenType::Other, "("); !res)
            return res;

        ExpressionNodePtr node;
        if (CouldBeType())
        {
            Type *type;
            if (auto res = ParseType() >> type; !res)
                return res;

            node = std::make_unique<SizeOfTypeExpressionNode>(type);
        }
        else
        {
            ExpressionNodePtr value;
            if (auto res = ParseExpressionNode() >> value; !res)
                return res;

            node = std::make_unique<SizeOfValueExpressionNode>(std::move(value));
        }

        if (auto res = Expect(TokenType::Other, ")"); !res)
            return res;

        return node;
    }

    if (Skip(TokenType::Other, "("))
    {
        ExpressionNodePtr node;

        if (CouldBeType())
        {
            Type *type;
            if (auto res = ParseType() >> type; !res)
                return res;

            if (auto res = Expect(TokenType::Other, ")"); !res)
                return res;

            ExpressionNodePtr operand;
            if (auto res = ParseOperandExpressionNode() >> operand; !res)
                return res;

            node = std::make_unique<CastExpressionNode>(type, std::move(operand));
        }
        else
        {
            if (auto res = ParseExpressionNode() >> node; !res)
                return res;

            if (auto res = Expect(TokenType::Other, ")"); !res)
                return res;
        }

        return node;
    }

    if (At(TokenType::Identifier))
    {
        auto name = Skip().Value;

        return { std::make_unique<SymbolExpressionNode>(std::move(name)) };
    }

    if (At(TokenType::Integer))
    {
        auto value = Skip().IntegerValue;

        return { std::make_unique<IntegerExpressionNode>(value) };
    }

    if (At(TokenType::FloatingPoint))
    {
        auto value = Skip().FloatingPointValue;

        return { std::make_unique<FloatingPointExpressionNode>(value) };
    }

    if (At(TokenType::String))
    {
        auto value = Skip().Value;

        return { std::make_unique<StringExpressionNode>(std::move(value)) };
    }

    return toolkit::make_error("TODO");
}

toolkit::result<scc::cc::Type *> scc::cc::Parser::ParseType()
{
    // TODO: const

    Type *aggregate;
    if (auto res = ParseBaseType() >> aggregate; !res)
        return res;

    for (;;)
    {
        if (Skip(TokenType::Other, "["))
        {
            if (Skip(TokenType::Other, "]"))
                aggregate = m_Context.GetArrayType(aggregate);
            else
            {
                uint64_t count;
                if (auto res = Expect(TokenType::Integer).extract(&Token::IntegerValue) >> count; !res)
                    return res;

                aggregate = m_Context.GetArrayType(aggregate, count);

                if (auto res = Expect(TokenType::Other, "]"); !res)
                    return res;
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

toolkit::result<scc::cc::Type *> scc::cc::Parser::ParseBaseType()
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

    if (Skip(TokenType::Identifier, "void"))
        return m_Context.GetVoidType();

    if (Skip(TokenType::Identifier, "bool"))
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
        return toolkit::make_error("TODO");
    if (short_count > 1)
        return toolkit::make_error("TODO");
    if (long_count > 2)
        return toolkit::make_error("TODO");

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

    return toolkit::make_error("TODO");
}

toolkit::result<scc::cc::Type *> scc::cc::Parser::ParseStructType()
{
    if (auto res = Expect(TokenType::Identifier, "struct"); !res)
        return res;

    std::optional<std::string> name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (name && !At(TokenType::Other, "{"))
        return m_Context.GetStructType(std::move(*name));

    if (auto res = Expect(TokenType::Other, "{"); !res)
        return res;

    std::vector<StructElement> elements;
    while (!At(TokenType::Other, "}") && !At(TokenType::None))
    {
        Type *element_type;
        if (auto res = ParseType() >> element_type; !res)
            return res;

        std::optional<std::string> element_name;
        if (At(TokenType::Identifier))
            element_name = Skip().Value;

        std::optional<uint8_t> element_bits;
        if (Skip(TokenType::Other, ":"))
        {
            uint64_t value;
            if (auto res = Expect(TokenType::Integer).extract(&Token::IntegerValue) >> value; !res)
                return res;

            element_bits = value;
        }

        if (auto res = Expect(TokenType::Other, ";"); !res)
            return res;

        elements.push_back(
            {
                .Ty = element_type,
                .Name = std::move(element_name),
                .Bits = element_bits,
            });
    }

    if (auto res = Expect(TokenType::Other, "}"); !res)
        return res;

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

toolkit::result<scc::cc::Type *> scc::cc::Parser::ParseUnionType()
{
    if (auto res = Expect(TokenType::Identifier, "union"); !res)
        return res;

    std::optional<std::string> name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (name && !At(TokenType::Other, "{"))
        return m_Context.GetUnionType(std::move(*name));

    if (auto res = Expect(TokenType::Other, "{"); !res)
        return res;

    std::vector<UnionElement> elements;
    while (!At(TokenType::Other, "}") && !At(TokenType::None))
    {
        Type *element_type;
        if (auto res = ParseType() >> element_type; !res)
            return res;

        std::optional<std::string> element_name;
        if (At(TokenType::Identifier))
            element_name = Skip().Value;

        if (auto res = Expect(TokenType::Other, ";"); !res)
            return res;

        elements.push_back(
            {
                .Ty = element_type,
                .Name = std::move(element_name),
            });
    }

    if (auto res = Expect(TokenType::Other, "}"); !res)
        return res;

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

toolkit::result<scc::cc::Type *> scc::cc::Parser::ParseEnumType()
{
    if (auto res = Expect(TokenType::Identifier, "enum"); !res)
        return res;

    std::optional<std::string> name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (name && !At(TokenType::Other, "{"))
        return m_Context.GetEnumType(std::move(*name));

    if (auto res = Expect(TokenType::Other, "{"); !res)
        return res;

    std::vector<EnumElement> elements;
    while (!At(TokenType::Other, "}") && !At(TokenType::None))
    {
        std::string element_name;
        if (auto res = Expect(TokenType::Identifier).extract(&Token::Value) >> element_name; !res)
            return res;

        std::optional<int64_t> element_value;
        if (Skip(TokenType::Operator, "="))
        {
            ExpressionNodePtr node;
            if (auto res = ParseExpressionNode() >> node; !res)
                return res;

            element_value = node->EvaluateConstantInteger();
        }

        if (!At(TokenType::Other, "}"))
            if (auto res = Expect(TokenType::Other, ","); !res)
                return res;

        elements.push_back(
            {
                .Name = std::move(element_name),
                .Value = element_value,
            });
    }

    if (auto res = Expect(TokenType::Other, "}"); !res)
        return res;

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

bool scc::cc::Parser::CouldBeType() const
{
    if (At(TokenType::Identifier, "struct"))
        return true;
    if (At(TokenType::Identifier, "union"))
        return true;
    if (At(TokenType::Identifier, "enum"))
        return true;

    if (At(TokenType::Identifier))
        if (m_Context.GetNamedType(m_Token.Value))
            return true;

    if (At(TokenType::Identifier, "void"))
        return true;
    if (At(TokenType::Identifier, "bool"))
        return true;
    if (At(TokenType::Identifier, "char"))
        return true;
    if (At(TokenType::Identifier, "int"))
        return true;
    if (At(TokenType::Identifier, "float"))
        return true;
    if (At(TokenType::Identifier, "double"))
        return true;
    if (At(TokenType::Identifier, "long"))
        return true;
    if (At(TokenType::Identifier, "short"))
        return true;
    if (At(TokenType::Identifier, "signed"))
        return true;
    if (At(TokenType::Identifier, "unsigned"))
        return true;

    return false;
}

bool scc::cc::Parser::At(const TokenType type) const
{
    return m_Token.Type == type;
}

bool scc::cc::Parser::At(const TokenType type, const std::string_view value) const
{
    return m_Token.Type == type && m_Token.Value == value;
}

toolkit::result<scc::cc::Token> scc::cc::Parser::Expect(TokenType type)
{
    if (At(type))
        return Skip();
    return toolkit::make_error("expected {}, got {}", type, m_Token.Type);
}

toolkit::result<> scc::cc::Parser::Expect(TokenType type, std::string_view value)
{
    if (Skip(type, value))
        return {};
    return toolkit::make_error("expected {} '{}', got {} '{}'", type, value, m_Token.Type, m_Token.Value);
}

bool scc::cc::Parser::Skip(const TokenType type)
{
    if (At(type))
    {
        m_Token = Next();
        return true;
    }
    return false;
}

bool scc::cc::Parser::Skip(const TokenType type, const std::string_view value)
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

static bool isdigit(const int c, const int base)
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
                    Error("failed to parse floating point value: {}", res.error());

                return { .Type = TokenType::FloatingPoint, .Raw = std::move(raw), .FloatingPointValue = val };
            }

            {
                uint64_t val;
                if (auto res = toolkit::parse_string<uint64_t>(value, base) >> val; !res)
                    Error("failed to parse integer value: {}", res.error());

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
                Error("character delimiters must only encapsulate a single character.");

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

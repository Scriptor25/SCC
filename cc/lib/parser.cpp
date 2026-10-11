#include <scc/cc/context.hpp>
#include <scc/cc/parser.hpp>
#include <scc/cc/type.hpp>

#include <scc/assert.hpp>

#include <toolkit/string.hxx>

#include <iostream>
#include <optional>
#include <unordered_map>
#include <unordered_set>

scc::cc::Parser::Parser(
    std::istream &stream,
    Context &context,
    Builder &builder)
    : m_Stream(stream),
      m_Context(context),
      m_Builder(builder)
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

        node->Generate(m_Builder);
    }
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseNode()
{
    if (At(TokenType::KeywordTypedef))
        return ParseTypeDefNode();

    const auto is_extern = Skip(TokenType::KeywordExtern);

    QualifiedType type;
    if (auto res = ParseType() >> type; !res)
        return res;

    std::string name;
    if (auto res = Expect(TokenType::Identifier).extract(&Token::Value) >> name; !res)
        return res;

    if (At(TokenType::LeftParenthesis))
        return ParseFunctionNode(is_extern, type, std::move(name));

    return ParseVariableNode(is_extern, type, std::move(name));
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseFunctionNode(
    bool is_extern,
    QualifiedType result,
    std::string name)
{
    if (auto res = Expect(TokenType::LeftParenthesis); !res)
        return res;

    std::vector<FunctionArgument> arguments;
    auto variadic = false;

    while (!At(TokenType::RightParenthesis))
    {
        if (!arguments.empty())
            if (auto res = Expect(TokenType::Comma); !res)
                return res;

        if (Skip(TokenType::Operator, "..."))
        {
            variadic = true;
            break;
        }

        QualifiedType argument_type;
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

    if (auto res = Expect(TokenType::RightParenthesis); !res)
        return res;

    if (Skip(TokenType::Semicolon))
        return {
            std::make_unique<FunctionNode>(
                is_extern,
                result,
                std::move(name),
                std::move(arguments),
                variadic)
        };

    StatementNodePtr content;
    if (auto res = ParseSequenceStatementNode() >> content; !res)
        return res;

    return {
        std::make_unique<FunctionNode>(
            is_extern,
            result,
            std::move(name),
            std::move(arguments),
            variadic,
            std::move(content))
    };
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseVariableNode(
    bool is_extern,
    QualifiedType type,
    std::string name)
{
    if (Skip(TokenType::Semicolon))
        return {
            std::make_unique<VariableNode>(
                is_extern,
                type,
                std::move(name))
        };

    ExpressionNodePtr value;
    if (auto res = ParseExpressionNode() >> value; !res)
        return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    return {
        std::make_unique<VariableNode>(
            is_extern,
            type,
            std::move(name),
            std::move(value))
    };
}

toolkit::result<scc::cc::NodePtr> scc::cc::Parser::ParseTypeDefNode()
{
    // TODO: handle function and array types:
    // TODO: typedef <result>(<name>)(<argument>...);
    // TODO: typedef <element> <name>[<count>];

    if (auto res = Expect(TokenType::KeywordTypedef); !res)
        return res;

    QualifiedType type;
    if (auto res = ParseType() >> type; !res)
        return res;

    std::string name;
    if (auto res = Expect(TokenType::Identifier).extract(&Token::Value) >> name; !res)
        return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    m_Context.SetNamedType(name, type);

    return { std::make_unique<TypeDefNode>(type, std::move(name)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseStatementNode(const bool ignore_end)
{
    if (At(TokenType::LeftBrace))
        return ParseSequenceStatementNode();

    if (At(TokenType::KeywordIf))
        return ParseIfStatementNode();

    if (At(TokenType::KeywordWhile))
        return ParseWhileStatementNode();
    if (At(TokenType::KeywordDo))
        return ParseDoWhileStatementNode();
    if (At(TokenType::KeywordFor))
        return ParseForStatementNode();

    if (At(TokenType::KeywordReturn))
        return ParseReturnStatementNode();
    if (At(TokenType::KeywordBreak))
        return ParseBreakStatementNode();
    if (At(TokenType::KeywordContinue))
        return ParseContinueStatementNode();

    if (CouldBeType())
        return ParseVariableStatementNode(ignore_end);

    ExpressionNodePtr node;
    if (auto res = ParseExpressionNode() >> node; !res)
        return res;

    if (!ignore_end)
        if (auto res = Expect(TokenType::Semicolon); !res)
            return res;

    return { std::make_unique<ExpressionStatementNode>(std::move(node)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseSequenceStatementNode()
{
    if (auto res = Expect(TokenType::LeftBrace); !res)
        return res;

    std::vector<StatementNodePtr> nodes;

    while (!At(TokenType::RightBrace))
    {
        StatementNodePtr node;
        if (auto res = ParseStatementNode() >> node; !res)
            return res;

        nodes.push_back(std::move(node));
    }

    if (auto res = Expect(TokenType::RightBrace); !res)
        return res;

    return { std::make_unique<SequenceStatementNode>(std::move(nodes)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseIfStatementNode()
{
    if (auto res = Expect(TokenType::KeywordIf); !res)
        return res;

    if (auto res = Expect(TokenType::LeftParenthesis); !res)
        return res;

    ExpressionNodePtr condition;
    if (auto res = ParseExpressionNode() >> condition; !res)
        return res;

    if (auto res = Expect(TokenType::RightParenthesis); !res)
        return res;

    StatementNodePtr then, else_;

    if (auto res = ParseStatementNode() >> then; !res)
        return res;

    if (Skip(TokenType::KeywordElse))
        if (auto res = ParseStatementNode() >> else_; !res)
            return res;

    return { std::make_unique<IfStatementNode>(std::move(condition), std::move(then), std::move(else_)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseWhileStatementNode()
{
    if (auto res = Expect(TokenType::KeywordWhile); !res)
        return res;

    if (auto res = Expect(TokenType::LeftParenthesis); !res)
        return res;

    ExpressionNodePtr condition;
    if (auto res = ParseExpressionNode() >> condition; !res)
        return res;

    if (auto res = Expect(TokenType::RightParenthesis); !res)
        return res;

    StatementNodePtr loop;

    if (auto res = ParseStatementNode() >> loop; !res)
        return res;

    return { std::make_unique<WhileStatementNode>(std::move(condition), std::move(loop)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseDoWhileStatementNode()
{
    if (auto res = Expect(TokenType::KeywordDo); !res)
        return res;

    StatementNodePtr loop;
    if (auto res = ParseStatementNode() >> loop; !res)
        return res;

    if (auto res = Expect(TokenType::KeywordWhile); !res)
        return res;

    if (auto res = Expect(TokenType::LeftParenthesis); !res)
        return res;

    ExpressionNodePtr condition;
    if (auto res = ParseExpressionNode() >> condition; !res)
        return res;

    if (auto res = Expect(TokenType::RightParenthesis); !res)
        return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    return { std::make_unique<DoWhileStatementNode>(std::move(loop), std::move(condition)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseForStatementNode()
{
    if (auto res = Expect(TokenType::KeywordFor); !res)
        return res;

    if (auto res = Expect(TokenType::LeftParenthesis); !res)
        return res;

    StatementNodePtr prefix;
    if (!At(TokenType::Semicolon))
        if (auto res = ParseStatementNode(true) >> prefix; !res)
            return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    ExpressionNodePtr condition;
    if (!At(TokenType::Semicolon))
        if (auto res = ParseExpressionNode() >> condition; !res)
            return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    StatementNodePtr suffix;
    if (!At(TokenType::RightParenthesis))
        if (auto res = ParseStatementNode(true) >> suffix; !res)
            return res;

    if (auto res = Expect(TokenType::RightParenthesis); !res)
        return res;

    StatementNodePtr loop;
    if (auto res = ParseStatementNode() >> loop; !res)
        return res;

    return {
        std::make_unique<ForStatementNode>(
            std::move(prefix),
            std::move(condition),
            std::move(suffix),
            std::move(loop))
    };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseReturnStatementNode()
{
    if (auto res = Expect(TokenType::KeywordReturn); !res)
        return res;

    ExpressionNodePtr value;
    if (!At(TokenType::Semicolon))
        if (auto res = ParseExpressionNode() >> value; !res)
            return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    return { std::make_unique<ReturnStatementNode>(std::move(value)) };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseBreakStatementNode()
{
    if (auto res = Expect(TokenType::KeywordBreak); !res)
        return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    return { std::make_unique<BreakStatementNode>() };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseContinueStatementNode()
{
    if (auto res = Expect(TokenType::KeywordContinue); !res)
        return res;

    if (auto res = Expect(TokenType::Semicolon); !res)
        return res;

    return { std::make_unique<ContinueStatementNode>() };
}

toolkit::result<scc::cc::StatementNodePtr> scc::cc::Parser::ParseVariableStatementNode(const bool ignore_end)
{
    QualifiedType type;
    if (auto res = ParseType() >> type; !res)
        return res;

    std::vector<VariableElement> elements;
    do
    {
        std::string name;
        if (auto res = Expect(TokenType::Identifier).extract(&Token::Value) >> name; !res)
            return res;

        ExpressionNodePtr val;
        if (Skip(TokenType::Operator, "="))
            if (auto res = ParseExpressionNode() >> val; !res)
                return res;

        // TODO: parse array element count

        elements.push_back(
            {
                .Name = std::move(name),
                .Val = std::move(val),
            });
    }
    while (Skip(TokenType::Comma));

    if (!ignore_end)
        if (auto res = Expect(TokenType::Semicolon); !res)
            return res;

    // TODO: parse volatility
    auto is_volatile = false;

    return { std::make_unique<VariableStatementNode>(type, std::move(elements), is_volatile) };
}

toolkit::result<scc::cc::ExpressionNodePtr> scc::cc::Parser::ParseExpressionNode()
{
    return ParseBinaryExpressionNode();
}

toolkit::result<scc::cc::ExpressionNodePtr> scc::cc::Parser::ParseBinaryExpressionNode()
{
    ExpressionNodePtr operand;
    if (auto res = ParseOperandExpressionNode() >> operand; !res)
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
        { "*", { false, 13, BinaryOperator::Mul } },
        { "/", { false, 13, BinaryOperator::Div } },
        { "%", { false, 13, BinaryOperator::Rem } },
        { "+", { false, 12, BinaryOperator::Add } },
        { "-", { false, 12, BinaryOperator::Sub } },
        { "<<", { false, 11, BinaryOperator::Shl } },
        { ">>", { false, 11, BinaryOperator::Shr } },
        { "<", { false, 10, BinaryOperator::CmpLt } },
        { "<=", { false, 10, BinaryOperator::CmpLe } },
        { ">", { false, 10, BinaryOperator::CmpGt } },
        { ">=", { false, 10, BinaryOperator::CmpGe } },
        { "==", { false, 9, BinaryOperator::CmpEq } },
        { "!=", { false, 9, BinaryOperator::CmpNe } },
        { "&", { false, 8, BinaryOperator::And } },
        { "^", { false, 7, BinaryOperator::Xor } },
        { "|", { false, 6, BinaryOperator::Or } },
        { "&&", { false, 5, BinaryOperator::LAnd } },
        { "||", { false, 4, BinaryOperator::LOr } },
        { "?", { true, 3 } },
        { "=", { true, 2, BinaryOperator::Assign } },
        { "+=", { true, 2, BinaryOperator::AddAssign } },
        { "-=", { true, 2, BinaryOperator::SubAssign } },
        { "*=", { true, 2, BinaryOperator::MulAssign } },
        { "/=", { true, 2, BinaryOperator::DivAssign } },
        { "%=", { true, 2, BinaryOperator::RemAssign } },
        { "<<=", { true, 2, BinaryOperator::ShlAssign } },
        { ">>=", { true, 2, BinaryOperator::ShrAssign } },
        { "&=", { true, 2, BinaryOperator::AndAssign } },
        { "^=", { true, 2, BinaryOperator::XorAssign } },
        { "|=", { true, 2, BinaryOperator::OrAssign } },
    };

    while (true)
    {
        const auto before_it = precedence.find(m_Token.Value);
        if (before_it == precedence.end() || before_it->second.Precedence < min_precedence)
            break;

        const auto before = before_it->second;

        auto o = Skip().Value;

        ExpressionNodePtr right;
        if (auto res = ParseOperandExpressionNode() >> right; !res)
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
            if (auto res = Expect(TokenType::Colon); !res)
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

        if (Skip(TokenType::LeftParenthesis))
        {
            std::vector<ExpressionNodePtr> arguments;

            while (!At(TokenType::RightParenthesis))
            {
                if (!arguments.empty())
                    if (auto res = Expect(TokenType::Comma); !res)
                        return res;

                ExpressionNodePtr argument;
                if (auto res = ParseExpressionNode() >> argument; !res)
                    return res;

                arguments.push_back(std::move(argument));
            }

            if (auto res = Expect(TokenType::RightParenthesis); !res)
                return res;

            operand = std::make_unique<CallExpressionNode>(std::move(operand), std::move(arguments));
            continue;
        }

        if (Skip(TokenType::LeftBracket))
        {
            ExpressionNodePtr index;
            if (auto res = ParseExpressionNode() >> index; !res)
                return res;

            if (auto res = Expect(TokenType::RightBracket); !res)
                return res;

            operand = std::make_unique<SubscriptExpressionNode>(std::move(operand), std::move(index));
            continue;
        }

        if (Skip(TokenType::Operator, "."))
        {
            Token token;
            if (auto res = Expect(TokenType::Identifier) >> token; !res)
                return res;

            auto name = std::move(token).Value;

            operand = std::make_unique<MemberExpressionNode>(std::move(operand), std::move(name), false);
            continue;
        }

        if (Skip(TokenType::Operator, "->"))
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
            Skip();

            ExpressionNodePtr operand;
            if (auto res = ParseOperandExpressionNode() >> operand; !res)
                return res;

            return { std::make_unique<UnaryExpressionNode>(it->second, std::move(operand)) };
        }
    }

    if (Skip(TokenType::KeywordSizeof))
    {
        if (auto res = Expect(TokenType::LeftParenthesis); !res)
            return res;

        ExpressionNodePtr node;
        if (CouldBeType())
        {
            QualifiedType type;
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

        if (auto res = Expect(TokenType::RightParenthesis); !res)
            return res;

        return node;
    }

    if (Skip(TokenType::LeftParenthesis))
    {
        ExpressionNodePtr node;

        if (CouldBeType())
        {
            QualifiedType type;
            if (auto res = ParseType() >> type; !res)
                return res;

            if (auto res = Expect(TokenType::RightParenthesis); !res)
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

            if (auto res = Expect(TokenType::RightParenthesis); !res)
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

toolkit::result<scc::cc::QualifiedType> scc::cc::Parser::ParseType()
{
    QualifiedType aggregate;
    if (auto res = ParseBaseType() >> aggregate; !res)
        return res;

    for (;;)
    {
        auto constness = Skip(TokenType::KeywordConst);

        if (Skip(TokenType::LeftBracket))
        {
            if (Skip(TokenType::RightBracket))
                aggregate = QualifiedType
                {
                    .Base = m_Context.GetArrayType(aggregate),
                    .Const = constness,
                };
            else
            {
                uint64_t count;
                if (auto res = Expect(TokenType::Integer).extract(&Token::IntegerValue) >> count; !res)
                    return res;

                aggregate = QualifiedType
                {
                    .Base = m_Context.GetArrayType(aggregate, count),
                    .Const = constness,
                };

                if (auto res = Expect(TokenType::RightBracket); !res)
                    return res;
            }

            continue;
        }

        if (Skip(TokenType::Operator, "*"))
        {
            if (Skip(TokenType::KeywordConst))
            {
                if (constness)
                    return toolkit::make_error("duplicate const keyword");
                constness = true;
            }

            aggregate = QualifiedType
            {
                .Base = m_Context.GetPointerType(aggregate),
                .Const = constness,
            };
            continue;
        }

        // TODO: function type

        break;
    }

    return aggregate;
}

toolkit::result<scc::cc::QualifiedType> scc::cc::Parser::ParseBaseType()
{
    auto constness = Skip(TokenType::KeywordConst);

    if (At(TokenType::KeywordStruct))
        return ParseStructType(constness);

    if (At(TokenType::KeywordUnion))
        return ParseUnionType(constness);

    if (At(TokenType::KeywordEnum))
        return ParseEnumType(constness);

    if (At(TokenType::Identifier))
        if (const auto type = m_Context.GetNamedType(m_Token.Value))
        {
            Skip();

            if (constness && type.Const)
                return toolkit::make_error("duplicate const keyword");

            constness = type.Const;

            if (Skip(TokenType::KeywordConst))
            {
                if (constness)
                    return toolkit::make_error("duplicate const keyword");
                constness = true;
            }

            return QualifiedType
            {
                .Base = type.Base,
                .Const = constness,
            };
        }

    if (Skip(TokenType::KeywordVoid))
    {
        if (Skip(TokenType::KeywordConst))
        {
            if (constness)
                return toolkit::make_error("duplicate const keyword");
            constness = true;
        }

        return QualifiedType
        {
            .Base = m_Context.GetVoidType(),
            .Const = constness,
        };
    }

    if (Skip(TokenType::KeywordBool))
    {
        if (Skip(TokenType::KeywordConst))
        {
            if (constness)
                return toolkit::make_error("duplicate const keyword");
            constness = true;
        }

        return QualifiedType
        {
            .Base = m_Context.GetBooleanType(),
            .Const = constness,
        };
    }

    enum class Base { None, Char, Int, Float, Double } base = Base::None;
    enum class Signedness { None, Signed, Unsigned } signedness = Signedness::None;
    unsigned short_count{}, long_count{};

    for (;;)
    {
        if (Skip(TokenType::KeywordChar))
        {
            base = Base::Char;
            continue;
        }

        if (Skip(TokenType::KeywordInt))
        {
            base = Base::Int;
            continue;
        }

        if (Skip(TokenType::KeywordFloat))
        {
            base = Base::Float;
            continue;
        }

        if (Skip(TokenType::KeywordDouble))
        {
            base = Base::Double;
            continue;
        }

        if (Skip(TokenType::KeywordSigned))
        {
            signedness = Signedness::Signed;
            continue;
        }

        if (Skip(TokenType::KeywordUnsigned))
        {
            signedness = Signedness::Unsigned;
            continue;
        }

        if (Skip(TokenType::KeywordShort))
        {
            ++short_count;
            continue;
        }

        if (Skip(TokenType::KeywordLong))
        {
            ++long_count;
            continue;
        }

        if (Skip(TokenType::KeywordConst))
        {
            if (constness)
                return toolkit::make_error("duplicate const keyword");
            constness = true;
            continue;
        }

        break;
    }

    if (short_count && long_count)
        return toolkit::make_error("short and long keywords");
    if (short_count > 1)
        return toolkit::make_error("duplicate short keyword");
    if (long_count > 2)
        return toolkit::make_error("duplicate long keyword");

    const Type *type{};
    switch (base)
    {
    case Base::None:
        switch (signedness)
        {
        case Signedness::None:
            if (short_count)
            {
                type = m_Context.GetIntegerType(IntegerKind::SignedShort);
                break;
            }
            switch (long_count)
            {
            case 1:
                type = m_Context.GetIntegerType(IntegerKind::SignedLong);
                break;
            case 2:
                type = m_Context.GetIntegerType(IntegerKind::SignedLongLong);
                break;
            default:
                break;
            }
            break;

        case Signedness::Signed:
            if (short_count)
            {
                type = m_Context.GetIntegerType(IntegerKind::SignedShort);
                break;
            }
            switch (long_count)
            {
            case 1:
                type = m_Context.GetIntegerType(IntegerKind::SignedLong);
                break;
            case 2:
                type = m_Context.GetIntegerType(IntegerKind::SignedLongLong);
                break;
            default:
                type = m_Context.GetIntegerType(IntegerKind::SignedInt);
                break;
            }
            break;

        case Signedness::Unsigned:
            if (short_count)
            {
                type = m_Context.GetIntegerType(IntegerKind::UnsignedShort);
                break;
            }
            switch (long_count)
            {
            case 1:
                type = m_Context.GetIntegerType(IntegerKind::UnsignedLong);
                break;
            case 2:
                type = m_Context.GetIntegerType(IntegerKind::UnsignedLongLong);
                break;
            default:
                type = m_Context.GetIntegerType(IntegerKind::UnsignedInt);
                break;
            }
            break;
        }
        break;

    case Base::Char:
        switch (signedness)
        {
        case Signedness::None:
            if (!short_count && !long_count)
                type = m_Context.GetIntegerType(IntegerKind::Char);
            break;

        case Signedness::Signed:
            if (!short_count && !long_count)
                type = m_Context.GetIntegerType(IntegerKind::SignedChar);
            break;

        case Signedness::Unsigned:
            if (!short_count && !long_count)
                type = m_Context.GetIntegerType(IntegerKind::UnsignedChar);
            break;
        }
        break;

    case Base::Int:
        switch (signedness)
        {
        case Signedness::None:
        case Signedness::Signed:
            if (short_count)
            {
                type = m_Context.GetIntegerType(IntegerKind::SignedShort);
                break;
            }
            switch (long_count)
            {
            case 1:
                type = m_Context.GetIntegerType(IntegerKind::SignedLong);
                break;
            case 2:
                type = m_Context.GetIntegerType(IntegerKind::SignedLongLong);
                break;
            default:
                type = m_Context.GetIntegerType(IntegerKind::SignedInt);
                break;
            }
            break;

        case Signedness::Unsigned:
            if (short_count)
            {
                type = m_Context.GetIntegerType(IntegerKind::UnsignedShort);
                break;
            }
            switch (long_count)
            {
            case 1:
                type = m_Context.GetIntegerType(IntegerKind::UnsignedLong);
                break;
            case 2:
                type = m_Context.GetIntegerType(IntegerKind::UnsignedLongLong);
                break;
            default:
                type = m_Context.GetIntegerType(IntegerKind::UnsignedInt);
                break;
            }
            break;
        }
        break;

    case Base::Float:
        if (signedness == Signedness::None && !short_count && !long_count)
            type = m_Context.GetFloatingPointType(FloatingPointKind::Float);
        break;

    case Base::Double:
        if (signedness == Signedness::None && !short_count)
            switch (long_count)
            {
            case 0:
                type = m_Context.GetFloatingPointType(FloatingPointKind::Double);
                break;
            case 1:
                type = m_Context.GetFloatingPointType(FloatingPointKind::LongDouble);
                break;
            default:
                break;
            }
        break;
    }

    if (type)
        return QualifiedType
        {
            .Base = type,
            .Const = constness,
        };

    return toolkit::make_error("TODO");
}

toolkit::result<scc::cc::QualifiedType> scc::cc::Parser::ParseStructType(bool constness)
{
    if (auto res = Expect(TokenType::KeywordStruct); !res)
        return res;

    std::string name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (!name.empty() && !Skip(TokenType::LeftBrace))
    {
        if (Skip(TokenType::KeywordConst))
        {
            if (constness)
                return toolkit::make_error("duplicate const keyword");
            constness = true;
        }

        return QualifiedType
        {
            .Base = m_Context.GetStructType(std::move(name)),
            .Const = constness,
        };
    }

    std::vector<StructElement> elements;
    while (!At(TokenType::RightBrace))
    {
        QualifiedType element_type;
        if (auto res = ParseType() >> element_type; !res)
            return res;

        std::optional<std::string> element_name;
        if (At(TokenType::Identifier))
            element_name = Skip().Value;

        std::optional<uint8_t> element_bits;
        if (Skip(TokenType::Colon))
        {
            uint64_t value;
            if (auto res = Expect(TokenType::Integer).extract(&Token::IntegerValue) >> value; !res)
                return res;

            element_bits = value;
        }

        if (auto res = Expect(TokenType::Semicolon); !res)
            return res;

        elements.push_back(
            {
                .Ty = element_type,
                .Name = std::move(element_name),
                .Bits = element_bits,
            });
    }

    if (auto res = Expect(TokenType::RightBrace); !res)
        return res;

    const StructType *type;
    if (name.empty())
        type = m_Context.GetStructType(std::move(elements));
    else
        type = m_Context.GetStructType(std::move(name), std::move(elements));

    if (Skip(TokenType::KeywordConst))
    {
        if (constness)
            return toolkit::make_error("duplicate const keyword");
        constness = true;
    }

    return QualifiedType
    {
        .Base = type,
        .Const = constness,
    };
}

toolkit::result<scc::cc::QualifiedType> scc::cc::Parser::ParseUnionType(bool constness)
{
    if (auto res = Expect(TokenType::KeywordUnion); !res)
        return res;

    std::string name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (!name.empty() && !Skip(TokenType::LeftBrace))
    {
        if (Skip(TokenType::KeywordConst))
        {
            if (constness)
                return toolkit::make_error("duplicate const keyword");
            constness = true;
        }

        return QualifiedType
        {
            .Base = m_Context.GetUnionType(std::move(name)),
            .Const = constness,
        };
    }

    std::vector<UnionElement> elements;
    while (!At(TokenType::RightBrace))
    {
        QualifiedType element_type;
        if (auto res = ParseType() >> element_type; !res)
            return res;

        std::optional<std::string> element_name;
        if (At(TokenType::Identifier))
            element_name = Skip().Value;

        if (auto res = Expect(TokenType::Semicolon); !res)
            return res;

        elements.push_back(
            {
                .Ty = element_type,
                .Name = std::move(element_name),
            });
    }

    if (auto res = Expect(TokenType::RightBrace); !res)
        return res;

    const UnionType *type;
    if (name.empty())
        type = m_Context.GetUnionType(std::move(elements));
    else
        type = m_Context.GetUnionType(std::move(name), std::move(elements));

    if (Skip(TokenType::KeywordConst))
    {
        if (constness)
            return toolkit::make_error("duplicate const keyword");
        constness = true;
    }

    return QualifiedType
    {
        .Base = type,
        .Const = constness,
    };
}

toolkit::result<scc::cc::QualifiedType> scc::cc::Parser::ParseEnumType(bool constness)
{
    if (auto res = Expect(TokenType::KeywordEnum); !res)
        return res;

    std::string name;
    if (At(TokenType::Identifier))
        name = Skip().Value;

    if (!name.empty() && !Skip(TokenType::LeftBrace))
    {
        if (Skip(TokenType::KeywordConst))
        {
            if (constness)
                return toolkit::make_error("duplicate const keyword");
            constness = true;
        }

        return QualifiedType
        {
            .Base = m_Context.GetEnumType(std::move(name)),
            .Const = constness
        };
    }

    std::vector<EnumElement> elements;
    while (!At(TokenType::RightBrace))
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

        if (!At(TokenType::RightBrace))
            if (auto res = Expect(TokenType::Comma); !res)
                return res;

        elements.push_back(
            {
                .Name = std::move(element_name),
                .Value = element_value,
            });
    }

    if (auto res = Expect(TokenType::RightBrace); !res)
        return res;

    const EnumType *type;
    if (name.empty())
        type = m_Context.GetEnumType(std::move(elements));
    else
        type = m_Context.GetEnumType(std::move(name), std::move(elements));

    if (Skip(TokenType::KeywordConst))
    {
        if (constness)
            return toolkit::make_error("duplicate const keyword");
        constness = true;
    }

    return QualifiedType
    {
        .Base = type,
        .Const = constness,
    };
}

bool scc::cc::Parser::CouldBeType() const
{
    if (At(TokenType::KeywordStruct))
        return true;
    if (At(TokenType::KeywordUnion))
        return true;
    if (At(TokenType::KeywordEnum))
        return true;

    if (At(TokenType::Identifier))
        if (m_Context.GetNamedType(m_Token.Value))
            return true;

    if (At(TokenType::KeywordVoid))
        return true;
    if (At(TokenType::KeywordBool))
        return true;
    if (At(TokenType::KeywordChar))
        return true;
    if (At(TokenType::KeywordInt))
        return true;
    if (At(TokenType::KeywordFloat))
        return true;
    if (At(TokenType::KeywordDouble))
        return true;
    if (At(TokenType::KeywordLong))
        return true;
    if (At(TokenType::KeywordShort))
        return true;
    if (At(TokenType::KeywordSigned))
        return true;
    if (At(TokenType::KeywordUnsigned))
        return true;

    if (At(TokenType::KeywordConst))
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
        None,
        LineComment,
        BlockComment,
        Operator,
        Identifier,
        Number,
        Char,
        String,
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
        { ".", { '.' } },
        { "..", { '.' } },
    };

    static const std::unordered_map<std::string_view, TokenType> keywords
    {
        { "auto", TokenType::KeywordAuto },
        { "break", TokenType::KeywordBreak },
        { "case", TokenType::KeywordCase },
        { "char", TokenType::KeywordChar },
        { "const", TokenType::KeywordConst },
        { "continue", TokenType::KeywordContinue },
        { "default", TokenType::KeywordDefault },
        { "do", TokenType::KeywordDo },
        { "double", TokenType::KeywordDouble },
        { "else", TokenType::KeywordElse },
        { "enum", TokenType::KeywordEnum },
        { "extern", TokenType::KeywordExtern },
        { "float", TokenType::KeywordFloat },
        { "for", TokenType::KeywordFor },
        { "goto", TokenType::KeywordGoto },
        { "if", TokenType::KeywordIf },
        { "int", TokenType::KeywordInt },
        { "long", TokenType::KeywordLong },
        { "register", TokenType::KeywordRegister },
        { "return", TokenType::KeywordReturn },
        { "short", TokenType::KeywordShort },
        { "signed", TokenType::KeywordSigned },
        { "sizeof", TokenType::KeywordSizeof },
        { "static", TokenType::KeywordStatic },
        { "struct", TokenType::KeywordStruct },
        { "switch", TokenType::KeywordSwitch },
        { "typedef", TokenType::KeywordTypedef },
        { "union", TokenType::KeywordUnion },
        { "unsigned", TokenType::KeywordUnsigned },
        { "void", TokenType::KeywordVoid },
        { "volatile", TokenType::KeywordVolatile },
        { "while", TokenType::KeywordWhile },

        { "inline", TokenType::KeywordInline },
        { "restrict", TokenType::KeywordRestrict },
        { "_Bool", TokenType::KeywordBool },
        { "_Complex", TokenType::KeywordComplex },
        { "_Imaginary", TokenType::KeywordImaginary },

        { "_Alignas", TokenType::KeywordAlignas },
        { "_Alignof", TokenType::KeywordAlignof },
        { "_Atomic", TokenType::KeywordAtomic },
        { "_Generic", TokenType::KeywordGeneric },
        { "_Noreturn", TokenType::KeywordNoreturn },
        { "_Static_assert", TokenType::KeywordStaticAssert },
        { "_Thread_local", TokenType::KeywordThreadLocal },

        { "alignas", TokenType::KeywordAlignas },
        { "alignof", TokenType::KeywordAlignof },
        { "bool", TokenType::KeywordBool },
        { "constexpr", TokenType::KeywordConstexpr },
        { "false", TokenType::KeywordFalse },
        { "nullptr", TokenType::KeywordNullptr },
        { "static_assert", TokenType::KeywordStaticAssert },
        { "thread_local", TokenType::KeywordThreadLocal },
        { "true", TokenType::KeywordTrue },
        { "typeof", TokenType::KeywordTypeof },
        { "typeof_unqual", TokenType::KeywordTypeofUnqual },
        { "_BitInt", TokenType::KeywordBitInt },
        { "_Decimal32", TokenType::KeywordDecimal32 },
        { "_Decimal64", TokenType::KeywordDecimal64 },
        { "_Decimal128", TokenType::KeywordDecimal128 },
    };

    static const std::unordered_map<std::string_view, TokenType> special
    {
        { ",", TokenType::Comma },
        { ";", TokenType::Semicolon },
        { ":", TokenType::Colon },

        { "(", TokenType::LeftParenthesis },
        { ")", TokenType::RightParenthesis },
        { "{", TokenType::LeftBrace },
        { "}", TokenType::RightBrace },
        { "[", TokenType::LeftBracket },
        { "]", TokenType::RightBracket },
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

            case '.':
                m_Buffer = push(true);
                if (isdigit(m_Buffer))
                {
                    base = 10;
                    floating_point = true;
                    state = State::Number;
                    break;
                }
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
            {
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

                TokenType type;
                if (const auto it = special.find(value); it != special.end())
                    type = it->second;
                else
                    type = TokenType::Other;

                return {
                    .Type = type,
                    .Raw = std::move(raw),
                    .Value = std::move(value),
                };
            }
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

            return {
                .Type = TokenType::Operator,
                .Raw = std::move(raw),
                .Value = std::move(value),
            };

        case State::Identifier:
        {
            if (std::isalnum(m_Buffer) || m_Buffer == '_')
            {
                m_Buffer = push(true);
                break;
            }

            TokenType type;
            if (const auto it = keywords.find(value); it != keywords.end())
                type = it->second;
            else
                type = TokenType::Identifier;

            return {
                .Type = type,
                .Raw = std::move(raw),
                .Value = std::move(value),
            };
        }

        case State::Number:
        {
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

                return {
                    .Type = TokenType::FloatingPoint,
                    .Raw = std::move(raw),
                    .FloatingPointValue = val,
                };
            }

            uint64_t val;
            if (auto res = toolkit::parse_string<uint64_t>(value, base) >> val; !res)
                Error("failed to parse integer value: {}", res.error());

            return {
                .Type = TokenType::Integer,
                .Raw = std::move(raw),
                .IntegerValue = val,
            };
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
                Error("character delimiters must only encapsulate a single character or escape sequence.");

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
            return {
                .Type = TokenType::String,
                .Raw = std::move(raw),
                .Value = std::move(value),
            };
        }
    }

    return {
        .Type = TokenType::None,
        .Raw = std::move(raw),
    };
}

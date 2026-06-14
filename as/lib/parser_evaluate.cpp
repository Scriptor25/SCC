#include <scc/as/align.hpp>
#include <scc/as/data.hpp>
#include <scc/as/fill.hpp>
#include <scc/as/operand.hpp>
#include <scc/as/parser.hpp>
#include <scc/as/section.hpp>
#include <scc/as/symbol.hpp>

#include <scc/bytes.hpp>
#include <scc/platform.hpp>

static toolkit::result<scc::as::OperandPtr> directive_set(
    const scc::as::EvaluationContext &,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".set requires 2 operands, got {}", operands.size());

    const auto *dst = dynamic_cast<scc::as::SymbolAddressOperand *>(operands[0].get());

    if (!dst)
        return toolkit::make_error("1st operand for .set must be symbol");

    auto *symbol = dst->GetSymbol();
    const auto value = operands[1]->GetImmediate();

    symbol->SetAddress(value);

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_add(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".add requires 2 operands, got {}", operands.size());

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, lhs + rhs) };
}

static toolkit::result<scc::as::OperandPtr> directive_sub(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".sub requires 2 operands, got {}", operands.size());

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, lhs - rhs) };
}

static toolkit::result<scc::as::OperandPtr> directive_shl(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".shl requires 2 operands, got {}", operands.size());

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, lhs << rhs) };
}

static toolkit::result<scc::as::OperandPtr> directive_or(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 2)
        return toolkit::make_error(".or requires 2 operands, got {}", operands.size());

    const auto lhs = operands[0]->GetImmediate();
    const auto rhs = operands[1]->GetImmediate();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, lhs | rhs) };
}

static toolkit::result<scc::as::OperandPtr> directive_neg(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".neg requires 1 operand, got {}", operands.size());

    const auto val = operands[0]->GetImmediate();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, -val) };
}

static toolkit::result<scc::as::OperandPtr> directive_length(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".length requires 1 operand, got {}", operands.size());

    const auto *src = dynamic_cast<scc::as::SymbolAddressOperand *>(operands[0].get());

    if (!src)
        return toolkit::make_error("1st operand for .length must be symbol");

    const auto *symbol = src->GetSymbol();
    const auto *fragment = dynamic_cast<scc::as::Data *>(symbol->GetFragment());

    auto value = fragment->GetDataSize();

    return { std::make_unique<scc::as::ImmediateOperand>(context.Platform, value) };
}

static toolkit::result<scc::as::OperandPtr> directive_fill(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() == 1)
    {
        auto count = operands[0]->GetImmediate();

        context.Section->Insert(std::make_unique<scc::as::Fill>(count, 1, 0));

        return {};
    }

    if (operands.size() == 2)
    {
        auto count = operands[0]->GetImmediate();
        auto value = operands[1]->GetImmediate();

        context.Section->Insert(std::make_unique<scc::as::Fill>(count, 1, value));

        return {};
    }

    if (operands.size() == 3)
    {
        auto count = operands[0]->GetImmediate();
        auto size = operands[1]->GetImmediate();
        auto value = operands[2]->GetImmediate();

        context.Section->Insert(std::make_unique<scc::as::Fill>(count, size, value));

        return {};
    }

    return toolkit::make_error(".fill requires 1, 2 or 3 operands, got {}", operands.size());
}

static toolkit::result<scc::as::OperandPtr> directive_align(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".align requires 1 operand, got {}", operands.size());

    auto alignment = operands[0]->GetImmediate();

    context.Section->Insert(std::make_unique<scc::as::Align>(alignment));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_skip(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".skip requires 1 operand, got {}", operands.size());

    auto count = operands[0]->GetImmediate();

    context.Section->Insert(std::make_unique<scc::as::Fill>(count, 1, 0));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_string(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".string requires 1 operand, got {}", operands.size());

    const auto *operand = dynamic_cast<scc::as::StringOperand *>(operands[0].get());

    auto &value = operand->GetValue();

    std::vector<uint8_t> data(value.size() + 1);
    for (size_t i = 0; i < value.size(); ++i)
        data[i] = value[i];
    data[value.size()] = 0;

    context.Section->Insert(std::make_unique<scc::as::Data>(std::move(data)));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_byte(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.empty())
        return toolkit::make_error(".byte requires at least 1 operand, got {}", operands.size());

    const auto value = operands[0]->GetImmediate();

    auto bytes = scc::ToBytes<1>(
        value,
        context.Platform.ISA.Endianness == scc::platform::TargetEndianness::LittleEndian);

    context.Section->Insert(std::make_unique<scc::as::Data>(bytes));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_word(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.empty())
        return toolkit::make_error(".word requires at least 1 operand, got {}", operands.size());

    const auto value = operands[0]->GetImmediate();

    auto bytes = scc::ToBytes<2>(
        value,
        context.Platform.ISA.Endianness == scc::platform::TargetEndianness::LittleEndian);

    context.Section->Insert(std::make_unique<scc::as::Data>(bytes));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_dword(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.empty())
        return toolkit::make_error(".dword requires at least 1 operand, got {}", operands.size());

    const auto value = operands[0]->GetImmediate();

    auto bytes = scc::ToBytes<4>(
        value,
        context.Platform.ISA.Endianness == scc::platform::TargetEndianness::LittleEndian);

    context.Section->Insert(std::make_unique<scc::as::Data>(bytes));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_qword(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.empty())
        return toolkit::make_error(".qword requires at least 1 operand, got {}", operands.size());

    const auto value = operands[0]->GetImmediate();

    auto bytes = scc::ToBytes<8>(
        value,
        context.Platform.ISA.Endianness == scc::platform::TargetEndianness::LittleEndian);

    context.Section->Insert(std::make_unique<scc::as::Data>(bytes));

    return {};
}

static toolkit::result<scc::as::OperandPtr> directive_extern(
    const scc::as::EvaluationContext &context,
    const std::vector<scc::as::OperandPtr> &operands)
{
    if (operands.size() != 1)
        return toolkit::make_error(".extern requires 1 operand, got {}", operands.size());

    const auto *operand = dynamic_cast<const scc::as::SymbolAddressOperand *>(operands[0].get());

    operand->GetSymbol()->SetExtern(true);

    return {};
}

toolkit::result<scc::as::OperandPtr> scc::as::Parser::Evaluate(
    const std::string &directive,
    const std::vector<OperandPtr> &operands) const
{
    static const std::unordered_map<std::string, toolkit::result<OperandPtr>(*)(
        const EvaluationContext &,
        const std::vector<OperandPtr> &)> map
    {
        { ".set", directive_set },
        { ".add", directive_add },
        { ".sub", directive_sub },
        { ".shl", directive_shl },
        { ".or", directive_or },
        { ".neg", directive_neg },
        { ".length", directive_length },
        { ".fill", directive_fill },
        { ".align", directive_align },
        { ".skip", directive_skip },
        { ".string", directive_string },
        { ".byte", directive_byte },
        { ".word", directive_word },
        { ".dword", directive_dword },
        { ".qword", directive_qword },
        { ".extern", directive_extern },
    };

    const auto it = map.find(directive);
    if (it == map.end())
        return toolkit::make_error("undefined directive '{}'", directive);

    return it->second(
        {
            .Platform = m_Platform,
            .Section = m_Section,
        },
        operands);
}

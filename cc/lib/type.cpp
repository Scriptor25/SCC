#include <scc/cc/context.hpp>
#include <scc/cc/type.hpp>

#include <scc/ir/context.hpp>

#include <unordered_map>

// TODO: depending on platform
static const std::unordered_map<scc::cc::IntegerKind, std::pair<bool, size_t>> integer_kind_info
{
    { scc::cc::IntegerKind::Bool, { false, 1 } },
    { scc::cc::IntegerKind::Char, { true, 8 } },
    { scc::cc::IntegerKind::SignedChar, { true, 8 } },
    { scc::cc::IntegerKind::UnsignedChar, { false, 8 } },
    { scc::cc::IntegerKind::SignedShort, { true, 16 } },
    { scc::cc::IntegerKind::UnsignedShort, { false, 16 } },
    { scc::cc::IntegerKind::SignedInt, { true, 32 } },
    { scc::cc::IntegerKind::UnsignedInt, { false, 32 } },
    { scc::cc::IntegerKind::SignedLong, { true, 64 } },
    { scc::cc::IntegerKind::UnsignedLong, { false, 64 } },
    { scc::cc::IntegerKind::SignedLongLong, { true, 64 } },
    { scc::cc::IntegerKind::UnsignedLongLong, { false, 64 } },
};

// TODO: depending on platform
static const std::unordered_map<scc::cc::FloatingPointKind, size_t> floating_point_kind_info
{
    { scc::cc::FloatingPointKind::Float, 32 },
    { scc::cc::FloatingPointKind::Double, 64 },
    { scc::cc::FloatingPointKind::LongDouble, 64 },
};

size_t scc::cc::VoidType::GetBits() const
{
    return 0;
}

scc::ir::Type *scc::cc::VoidType::Generate(Context &context) const
{
    return context.GetIRContext().GetVoidType();
}

scc::cc::IntegerType::IntegerType(IntegerKind kind)
    : Kind(kind)
{
}

size_t scc::cc::IntegerType::GetBits() const
{
    return integer_kind_info.at(Kind).second;
}

scc::ir::Type *scc::cc::IntegerType::Generate(Context &context) const
{
    const auto bits = GetBits();

    return context.GetIRContext().GetIntNType(bits);
}

scc::cc::FloatingPointType::FloatingPointType(FloatingPointKind kind)
    : Kind(kind)
{
}

size_t scc::cc::FloatingPointType::GetBits() const
{
    return floating_point_kind_info.at(Kind);
}

scc::ir::Type *scc::cc::FloatingPointType::Generate(Context &context) const
{
    const auto bits = GetBits();

    return context.GetIRContext().GetFloatNType(bits);
}

scc::cc::StructType::StructType(std::string name)
    : Name(std::move(name))
{
}

size_t scc::cc::StructType::GetBits() const
{
    size_t bits = 0;
    for (auto &element : Elements)
        bits += element.Ty->GetBits();

    // TODO: per-element alignment and size

    return bits;
}

scc::ir::Type *scc::cc::StructType::Generate(Context &context) const
{
    std::vector<ir::Type *> elements(Elements.size());
    for (size_t i = 0; i < elements.size(); ++i)
        elements[i] = Elements[i].Ty->Generate(context);

    // TODO: per-element alignment and size

    return context.GetIRContext().GetStructType(std::move(elements));
}

scc::cc::UnionType::UnionType(std::string name)
    : Name(std::move(name))
{
}

size_t scc::cc::UnionType::GetBits() const
{
    size_t bits = 0;
    for (auto &element : Elements)
        if (const auto element_bits = element.Ty->GetBits(); element_bits > bits)
            bits = element_bits;

    // TODO: per-element alignment and size

    return bits;
}

scc::ir::Type *scc::cc::UnionType::Generate(Context &context) const
{
    const auto bits = GetBits();
    const auto bytes = (bits + 0b111) & ~0b111;

    auto *element = context.GetIRContext().GetInt8Type();

    return context.GetIRContext().GetArrayType(element, bytes);
}

scc::cc::EnumType::EnumType(std::string name)
    : Name(std::move(name))
{
}

size_t scc::cc::EnumType::GetBits() const
{
    auto [_, bits] = DetermineType();

    return bits;
}

scc::ir::Type *scc::cc::EnumType::Generate(Context &context) const
{
    auto [_, bits] = DetermineType();

    return context.GetIRContext().GetIntNType(bits);
}

std::pair<bool, size_t> scc::cc::EnumType::DetermineType() const
{
    if (TypeOverride)
        return integer_kind_info.at(TypeOverride->Kind);

    int64_t min = INT64_MAX, max = INT64_MIN, current = 0;

    for (auto &[name, value] : Elements)
    {
        if (value)
            current = *value;
        else
            // TODO: warning if current will overflow
            // TODO: maybe use n-int in future?
            ++current;

        if (current > max)
            max = current;
        if (current < min)
            min = current;
    }

    bool is_signed{};
    uint8_t bits = 8;

    for (; bits <= 64; bits <<= 1)
    {
        const int64_t imin = bits == 64 ? INT64_MIN : -(1LL << (bits - 1));
        const int64_t imax = bits == 64 ? INT64_MAX : (1LL << (bits - 1)) - 1;

        if (imin <= min && max <= imax)
        {
            is_signed = true;
            break;
        }

        const uint64_t umax = bits == 64 ? UINT64_MAX : ((1ULL << bits) - 1);

        if (0 <= min && max <= umax)
        {
            is_signed = false;
            break;
        }
    }

    return { is_signed, bits };
}

scc::cc::PointerType::PointerType(Type *element)
    : Element(element)
{
}

size_t scc::cc::PointerType::GetBits() const
{
    // TODO: depending on platform
    return 64;
}

scc::ir::Type *scc::cc::PointerType::Generate(Context &context) const
{
    ir::Type *element;
    if (Element)
        element = Element->Generate(context);
    else
        element = context.GetIRContext().GetVoidType();

    return context.GetIRContext().GetPointerType(element);
}

scc::cc::ArrayType::ArrayType(Type *element, size_t count)
    : Element(element),
      Count(count)
{
}

size_t scc::cc::ArrayType::GetBits() const
{
    return Element->GetBits() * Count;
}

scc::ir::Type *scc::cc::ArrayType::Generate(Context &context) const
{
    auto *element = Element->Generate(context);
    return context.GetIRContext().GetArrayType(element, Count);
}

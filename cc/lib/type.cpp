#include <scc/cc/builder.hpp>
#include <scc/cc/context.hpp>
#include <scc/cc/type.hpp>

#include <unordered_map>
#include <scc/platform.hpp>

namespace
{
    struct IntegerKindInfo
    {
        bool Signed;
        size_t BitWidth;
        size_t Rank;
        scc::cc::IntegerKind Swap;
    };
}

// TODO: depending on platform
static const std::unordered_map<scc::cc::IntegerKind, IntegerKindInfo> integer_kind_info
{
    { scc::cc::IntegerKind::Bool, { false, 1, 1, scc::cc::IntegerKind::Bool } },
    { scc::cc::IntegerKind::Char, { true, 8, 2, scc::cc::IntegerKind::Char } },
    { scc::cc::IntegerKind::SignedChar, { true, 8, 2, scc::cc::IntegerKind::UnsignedChar } },
    { scc::cc::IntegerKind::UnsignedChar, { false, 8, 2, scc::cc::IntegerKind::SignedChar } },
    { scc::cc::IntegerKind::SignedShort, { true, 16, 3, scc::cc::IntegerKind::UnsignedShort } },
    { scc::cc::IntegerKind::UnsignedShort, { false, 16, 3, scc::cc::IntegerKind::SignedShort } },
    { scc::cc::IntegerKind::SignedInt, { true, 32, 4, scc::cc::IntegerKind::UnsignedInt } },
    { scc::cc::IntegerKind::UnsignedInt, { false, 32, 4, scc::cc::IntegerKind::SignedInt } },
    { scc::cc::IntegerKind::SignedLong, { true, 64, 5, scc::cc::IntegerKind::UnsignedLong } },
    { scc::cc::IntegerKind::UnsignedLong, { false, 64, 5, scc::cc::IntegerKind::SignedLong } },
    { scc::cc::IntegerKind::SignedLongLong, { true, 64, 6, scc::cc::IntegerKind::UnsignedLongLong } },
    { scc::cc::IntegerKind::UnsignedLongLong, { false, 64, 6, scc::cc::IntegerKind::SignedLongLong } },
};

// TODO: depending on platform
static const std::unordered_map<scc::cc::FloatingPointKind, size_t> floating_point_kind_info
{
    { scc::cc::FloatingPointKind::Float, 32 },
    { scc::cc::FloatingPointKind::Double, 64 },
    { scc::cc::FloatingPointKind::LongDouble, 64 },
};

scc::cc::Type *scc::cc::Type::Collapse(Context &context, Type *left, Type *right)
{
    if (left == right)
        return left;

    if (dynamic_cast<VoidType *>(left) || dynamic_cast<VoidType *>(right))
        return nullptr;

    if (auto *left_integer = dynamic_cast<IntegerType *>(left))
    {
        if (auto *right_integer = dynamic_cast<IntegerType *>(right))
        {
            if (left_integer->IsSigned() == right_integer->IsSigned())
            {
                if (left_integer->GetBitWidth() > right_integer->GetBitWidth())
                    return left;
                return right;
            }

            IntegerType *signed_type, *unsigned_type;
            if (left_integer->IsSigned())
            {
                signed_type = left_integer;
                unsigned_type = right_integer;
            }
            else
            {
                signed_type = right_integer;
                unsigned_type = left_integer;
            }

            if (unsigned_type->GetRank() >= signed_type->GetRank())
                return unsigned_type;

            if (signed_type->GetBitWidth() > unsigned_type->GetBitWidth())
                return signed_type;

            return context.GetIntegerType(signed_type->GetSwap());
        }

        if (auto *right_floating_point = dynamic_cast<FloatingPointType *>(right))
        {
        }

        if (auto *right_enum = dynamic_cast<EnumType *>(right))
        {
        }
    }

    if (auto *left_floating_point = dynamic_cast<FloatingPointType *>(left))
    {
        if (auto *right_integer = dynamic_cast<IntegerType *>(right))
        {
        }

        if (auto *right_floating_point = dynamic_cast<FloatingPointType *>(right))
        {
        }
    }

    if (auto *left_enum = dynamic_cast<EnumType *>(left))
    {
    }

    if (auto *left_pointer = dynamic_cast<PointerType *>(left))
    {
    }

    if (auto *left_array = dynamic_cast<ArrayType *>(left))
    {
        if (auto *right_pointer = dynamic_cast<PointerType *>(right))
        {
            if (dynamic_cast<VoidType *>(right_pointer->Element) || left_array->Element == right_pointer->Element)
                return right_pointer;

            return nullptr;
        }
    }

    IntegerType;
    FloatingPointType;
    UnionType;
    EnumType;
    PointerType;
    ArrayType;
}

size_t scc::cc::VoidType::GetBitWidth(Context &context) const
{
    return 0;
}

scc::ir::VoidType *scc::cc::VoidType::Generate(Builder &builder) const
{
    return builder.GetIRContext().GetVoidType();
}

scc::cc::IntegerType::IntegerType(const IntegerKind kind)
    : Kind(kind)
{
}

size_t scc::cc::IntegerType::GetBitWidth(Context &context) const
{
    // TODO: platform dependent

    return integer_kind_info.at(Kind).BitWidth;
}

scc::ir::IntType *scc::cc::IntegerType::Generate(Builder &builder) const
{
    const auto bit_width = GetBitWidth(builder.GetContext());

    return builder.GetIRContext().GetIntNType(bit_width);
}

bool scc::cc::IntegerType::IsSigned() const
{
    return integer_kind_info.at(Kind).Signed;
}

size_t scc::cc::IntegerType::GetBitWidth() const
{
    return integer_kind_info.at(Kind).BitWidth;
}

size_t scc::cc::IntegerType::GetRank() const
{
    return integer_kind_info.at(Kind).Rank;
}

scc::cc::IntegerKind scc::cc::IntegerType::GetSwap() const
{
    return integer_kind_info.at(Kind).Swap;
}

scc::cc::FloatingPointType::FloatingPointType(const FloatingPointKind kind)
    : Kind(kind)
{
}

size_t scc::cc::FloatingPointType::GetBitWidth(Context &context) const
{
    return floating_point_kind_info.at(Kind);
}

scc::ir::FloatType *scc::cc::FloatingPointType::Generate(Builder &builder) const
{
    const auto bit_width = GetBitWidth(builder.GetContext());

    return builder.GetIRContext().GetFloatNType(bit_width);
}

scc::cc::StructType::StructType(std::string name)
    : Name(std::move(name))
{
}

size_t scc::cc::StructType::GetBitWidth(Context &context) const
{
    size_t bit_width = 0;
    for (auto &element : Elements)
        bit_width += element.Ty->GetBitWidth(context);

    // TODO: element alignment and size

    return bit_width;
}

scc::ir::StructType *scc::cc::StructType::Generate(Builder &builder) const
{
    std::vector<ir::Type *> elements(Elements.size());
    for (size_t i = 0; i < elements.size(); ++i)
        elements[i] = Elements[i].Ty->Generate(builder);

    // TODO: element alignment and size

    return builder.GetIRContext().GetStructType(std::move(elements));
}

scc::cc::UnionType::UnionType(std::string name)
    : Name(std::move(name))
{
}

size_t scc::cc::UnionType::GetBitWidth(Context &context) const
{
    size_t bit_width = 0;
    for (auto &element : Elements)
        if (const auto element_bit_width = element.Ty->GetBitWidth(context); element_bit_width > bit_width)
            bit_width = element_bit_width;

    // TODO: element alignment and size

    return bit_width;
}

scc::ir::ArrayType *scc::cc::UnionType::Generate(Builder &builder) const
{
    const auto bit_width = GetBitWidth(builder.GetContext());
    const auto byte_count = (bit_width + 0b111) & ~0b111;

    auto *element = builder.GetIRContext().GetInt8Type();

    return builder.GetIRContext().GetArrayType(element, byte_count);
}

scc::cc::EnumType::EnumType(std::string name)
    : Name(std::move(name))
{
}

size_t scc::cc::EnumType::GetBitWidth(Context &context) const
{
    return DetermineType(context)->GetBitWidth(context);
}

scc::ir::IntType *scc::cc::EnumType::Generate(Builder &builder) const
{
    return DetermineType(builder.GetContext())->Generate(builder);
}

scc::cc::IntegerType *scc::cc::EnumType::DetermineType(Context &context) const
{
    if (TypeOverride)
        return TypeOverride;

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
    uint8_t bit_width = 8;

    for (; bit_width <= 64; bit_width <<= 1)
    {
        const int64_t imin = bit_width == 64 ? INT64_MIN : -(1LL << (bit_width - 1));
        const int64_t imax = bit_width == 64 ? INT64_MAX : (1LL << (bit_width - 1)) - 1;

        if (imin <= min && max <= imax)
        {
            is_signed = true;
            break;
        }

        const uint64_t umax = bit_width == 64 ? UINT64_MAX : ((1ULL << bit_width) - 1);

        if (0 <= min && max <= umax)
        {
            is_signed = false;
            break;
        }
    }

    IntegerKind kind;
    switch (bit_width)
    {
    case 8:
        kind = is_signed ? IntegerKind::SignedChar : IntegerKind::UnsignedChar;
        break;
    case 16:
        kind = is_signed ? IntegerKind::SignedShort : IntegerKind::UnsignedShort;
        break;
    case 32:
        kind = is_signed ? IntegerKind::SignedInt : IntegerKind::UnsignedInt;
        break;
    case 64:
        kind = is_signed ? IntegerKind::SignedLong : IntegerKind::UnsignedLong;
        break;

    default:
        kind = is_signed ? IntegerKind::SignedLongLong : IntegerKind::UnsignedLongLong;
        break;
    }

    return context.GetIntegerType(kind);
}

scc::cc::PointerType::PointerType(Type *element)
    : Element(element)
{
}

size_t scc::cc::PointerType::GetBitWidth(Context &context) const
{
    return context.GetPlatform().ABI.DataLayout.PointerSize * 8;
}

scc::ir::PointerType *scc::cc::PointerType::Generate(Builder &builder) const
{
    ir::Type *element;
    if (Element)
        element = Element->Generate(builder);
    else
        element = builder.GetIRContext().GetVoidType();

    return builder.GetIRContext().GetPointerType(element);
}

scc::cc::ArrayType::ArrayType(Type *element, const size_t count)
    : Element(element),
      Count(count)
{
}

size_t scc::cc::ArrayType::GetBitWidth(Context &context) const
{
    // TODO: element alignment and size

    return Element->GetBitWidth(context) * Count;
}

scc::ir::ArrayType *scc::cc::ArrayType::Generate(Builder &builder) const
{
    auto *element = Element->Generate(builder);
    return builder.GetIRContext().GetArrayType(element, Count);
}

scc::cc::FunctionType::FunctionType(
    Type *result,
    std::vector<Type *> arguments,
    const bool variadic)
    : Result(result),
      Arguments(std::move(arguments)),
      Variadic(variadic)
{
}

size_t scc::cc::FunctionType::GetBitWidth(Context &context) const
{
    // function pointer
    return context.GetPlatform().ABI.DataLayout.PointerSize * 8;
}

scc::ir::FunctionType *scc::cc::FunctionType::Generate(Builder &builder) const
{
    auto *result = Result->Generate(builder);

    std::vector<ir::Type *> arguments(Arguments.size());
    for (size_t i = 0; i < Arguments.size(); ++i)
        arguments[i] = Arguments[i]->Generate(builder);

    return builder.GetIRContext().GetFunctionType(result, std::move(arguments), Variadic);
}

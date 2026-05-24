#include <scc/platform.hpp>
#include <scc/platform/riscv.hpp>
#include <scc/platform/x86.hpp>

std::string_view scc::platform::RegisterView::CanonicalName() const
{
    return Names[0];
}

std::string_view scc::platform::MnemonicDescriptor::CanonicalName() const
{
    return Names[0];
}

const scc::platform::RegisterView *scc::platform::InstructionSetArchitecture::FindRegisterView(
    const Register key) const
{
    if (const auto it = RegisterViews.find(key); it != RegisterViews.end())
        return &it->second;
    return nullptr;
}

const scc::platform::RegisterView *scc::platform::InstructionSetArchitecture::FindRegisterView(
    const std::string_view key) const
{
    if (const auto it = RegisterNames.find(key); it != RegisterNames.end())
        return FindRegisterView(it->second);
    return nullptr;
}

std::optional<scc::Register> scc::platform::InstructionSetArchitecture::FindRegisterName(
    const std::string_view key) const
{
    if (const auto it = RegisterNames.find(key); it != RegisterNames.end())
        return it->second;
    return std::nullopt;
}

const scc::platform::MnemonicDescriptor *scc::platform::InstructionSetArchitecture::FindMnemonic(
    const Mnemonic key) const
{
    if (const auto it = Mnemonics.find(key); it != Mnemonics.end())
        return &it->second;
    return nullptr;
}

const scc::platform::MnemonicDescriptor *scc::platform::InstructionSetArchitecture::FindMnemonic(
    const std::string_view key) const
{
    if (const auto it = MnemonicNames.find(key); it != MnemonicNames.end())
        return FindMnemonic(it->second);
    return nullptr;
}

std::optional<scc::Mnemonic> scc::platform::InstructionSetArchitecture::FindMnemonicName(
    const std::string_view key) const
{
    if (const auto it = MnemonicNames.find(key); it != MnemonicNames.end())
        return it->second;
    return std::nullopt;
}

const std::unordered_map<
    std::string_view,
    std::pair<scc::platform::TargetArchitecture, uint8_t>
> scc::platform::architectures
{
    { "x86", { TargetArchitecture::X86, 32 } },
    { "i386", { TargetArchitecture::X86, 32 } },
    { "i486", { TargetArchitecture::X86, 32 } },
    { "i586", { TargetArchitecture::X86, 32 } },
    { "i686", { TargetArchitecture::X86, 32 } },
    { "x86_64", { TargetArchitecture::X86, 64 } },

    { "ppc", { TargetArchitecture::PPC, 32 } },
    { "ppc32", { TargetArchitecture::PPC, 32 } },
    { "ppc64", { TargetArchitecture::PPC, 64 } },
    { "ppc64le", { TargetArchitecture::PPC, 64 } },

    { "arm", { TargetArchitecture::ARM, 32 } },
    { "armv7", { TargetArchitecture::ARM, 32 } },
    { "thumb", { TargetArchitecture::ARM, 32 } },
    { "aarch32", { TargetArchitecture::ARM, 32 } },
    { "aarch64", { TargetArchitecture::ARM, 64 } },

    { "riscv32", { TargetArchitecture::RiscV, 32 } },
    { "riscv64", { TargetArchitecture::RiscV, 64 } },
};

const std::unordered_map<std::string_view, scc::platform::TargetSystem> scc::platform::systems
{
    { "none", TargetSystem::None },
    { "linux", TargetSystem::Linux },
    { "windows", TargetSystem::Windows },
    { "win32", TargetSystem::Windows },
    { "darwin", TargetSystem::Darwin },
    { "macos", TargetSystem::Darwin },
    { "ios", TargetSystem::Darwin },
};

const std::unordered_map<std::string_view, scc::platform::TargetEnvironment> scc::platform::environments
{
    { "eabi", TargetEnvironment::EABI },
    { "gnu", TargetEnvironment::GNU },
    { "android", TargetEnvironment::Android },
    { "macho", TargetEnvironment::MachO },
    { "elf", TargetEnvironment::ELF },
};

static std::vector<std::string_view> split(const std::string_view str)
{
    std::vector<std::string_view> result;

    size_t b, e;
    for (b = 0, e = 0; (e = str.find('-', b)) != std::string_view::npos; b = e + 1)
        if (b != e)
            result.push_back(str.substr(b, e - b));

    if (b != e)
        result.push_back(str.substr(b));

    return result;
}

toolkit::result<scc::Platform> scc::platform::TargetTriple::GetPlatform() const
{
    Platform result;
    if (auto res = GetIR() >> result.IR; !res)
        return res;
    if (auto res = GetISA() >> result.ISA; !res)
        return res;
    if (auto res = GetABI() >> result.ABI; !res)
        return res;
    return result;
}

toolkit::result<scc::platform::IntermediateRepresentation> scc::platform::TargetTriple::GetIR() const
{
    switch (Architecture)
    {
    case TargetArchitecture::X86:
        return x86::CreateIntermediateRepresentation({ .BitWidth = BitWidth });

    case TargetArchitecture::PPC:
        return IntermediateRepresentation
        {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64 },
        };

    case TargetArchitecture::ARM:
        return IntermediateRepresentation
        {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 16, 32, 64 },
        };

    case TargetArchitecture::RiscV:
        return riscv::CreateIntermediateRepresentation({ .BitWidth = BitWidth });

    default:
        return toolkit::make_error("invalid architecture {}", Architecture);
    }
}

toolkit::result<scc::platform::InstructionSetArchitecture> scc::platform::TargetTriple::GetISA() const
{
    InstructionSetArchitecture isa;

    switch (Architecture)
    {
    case TargetArchitecture::X86:
        if (auto res = x86::CreateInstructionSetArchitecture({ .BitWidth = BitWidth }) >> isa; !res)
            return res;
        break;

    case TargetArchitecture::PPC:
        isa = {
            .Architecture = TargetArchitecture::PPC,
            .Endianness = TargetEndianness::BigEndian,
            .BitWidth = BitWidth,
            .Registers = {},
            .RegisterViews = {},
            .Mnemonics = {},
        };
        break;

    case TargetArchitecture::ARM:
        isa = {
            .Architecture = TargetArchitecture::ARM,
            .Endianness = TargetEndianness::LittleEndian,
            .BitWidth = BitWidth,
            .Registers = {},
            .RegisterViews = {},
            .Mnemonics = {},
        };
        break;

    case TargetArchitecture::RiscV:
        if (auto res = riscv::CreateInstructionSetArchitecture({ .BitWidth = BitWidth }) >> isa; !res)
            return res;
        break;

    default:
        return toolkit::make_error("invalid architecture {}", Architecture);
    }

    for (auto &[fst, snd] : isa.RegisterViews)
        for (auto name : snd.Names)
            isa.RegisterNames[name] = fst;

    for (auto &[fst, snd] : isa.Mnemonics)
        for (auto name : snd.Names)
            isa.MnemonicNames[name] = fst;

    return isa;
}

toolkit::result<scc::platform::ApplicationBinaryInterface> scc::platform::TargetTriple::GetABI() const
{
    switch (System)
    {
    case TargetSystem::None:
        return ApplicationBinaryInterface
        {
            .DataLayout = {
                .PointerSize = BitWidth == 32 ? 4u : 8u,
                .PointerAlign = BitWidth == 32 ? 4u : 8u,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
                .HasRedZone = false,
                .CallingConvention = Architecture == TargetArchitecture::ARM
                                         ? TargetCallingConvention::AAPCS
                                         : Architecture == TargetArchitecture::RiscV
                                         ? TargetCallingConvention::RiscVCall
                                         : TargetCallingConvention::SystemV,
            },
            .ObjectFormat = TargetObjectFormat::ELF,
        };

    case TargetSystem::Linux:
        return ApplicationBinaryInterface
        {
            .DataLayout = {
                .PointerSize = BitWidth == 32 ? 4u : 8u,
                .PointerAlign = BitWidth == 32 ? 4u : 8u,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
                .HasRedZone = Architecture == TargetArchitecture::X86,
                .CallingConvention = Architecture == TargetArchitecture::ARM
                                         ? TargetCallingConvention::AAPCS
                                         : Architecture == TargetArchitecture::RiscV
                                         ? TargetCallingConvention::RiscVCall
                                         : TargetCallingConvention::SystemV,
            },
            .ObjectFormat = TargetObjectFormat::ELF,
        };

    case TargetSystem::Windows:
        return ApplicationBinaryInterface
        {
            .DataLayout = {
                .PointerSize = 8,
                .PointerAlign = 8,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
                .HasRedZone = false,
                .CallingConvention = TargetCallingConvention::MSX64,
            },
            .ObjectFormat = TargetObjectFormat::PE,
        };

    case TargetSystem::Darwin:
        return ApplicationBinaryInterface
        {
            .DataLayout = {
                .PointerSize = 8,
                .PointerAlign = 8,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
                .HasRedZone = true,
                .CallingConvention = TargetCallingConvention::SystemV,
            },
            .ObjectFormat = TargetObjectFormat::MachO,
        };

    default:
        return toolkit::make_error("invalid system {}", System);
    }
}

toolkit::result<scc::platform::TargetTriple> scc::platform::ParseTriple(const std::string_view triple)
{
    const auto segments = split(triple);
    if (segments.size() != 3)
        return toolkit::make_error("invalid segment count {}, expected 3", segments.size());

    TargetTriple result
    {
        .Name = std::string(triple),
    };

    if (const auto it = architectures.find(segments[0]); it != architectures.end())
    {
        result.Architecture = it->second.first;
        result.BitWidth = it->second.second;
    }
    else
        return toolkit::make_error("invalid architecture segment '{}'", segments[0]);

    if (const auto it = systems.find(segments[1]); it != systems.end())
        result.System = it->second;
    else
        return toolkit::make_error("invalid system segment '{}'", segments[1]);

    if (const auto it = environments.find(segments[2]); it != environments.end())
        result.Environment = it->second;
    else
        return toolkit::make_error("invalid environment segment '{}'", segments[2]);

    return result;
}

#include <scc/platform.hpp>

#include <stdexcept>
#include <vector>

const std::unordered_map<std::string_view, scc::platform::TargetArchitecture> scc::platform::TargetTriple::architectures
{
    { "x86", TargetArchitecture::X86 },
    { "i386", TargetArchitecture::X86 },
    { "i486", TargetArchitecture::X86 },
    { "i586", TargetArchitecture::X86 },
    { "i686", TargetArchitecture::X86 },
    { "x86_64", TargetArchitecture::X86 },

    { "ppc", TargetArchitecture::PPC },
    { "ppc32", TargetArchitecture::PPC },
    { "ppc64", TargetArchitecture::PPC },
    { "ppc64le", TargetArchitecture::PPC },

    { "arm", TargetArchitecture::ARM },
    { "armv7", TargetArchitecture::ARM },
    { "thumb", TargetArchitecture::ARM },
    { "aarch32", TargetArchitecture::ARM },
    { "aarch64", TargetArchitecture::ARM },

    { "riscv32", TargetArchitecture::RiscV },
    { "riscv64", TargetArchitecture::RiscV },
};

const std::unordered_map<std::string_view, scc::platform::TargetSystem> scc::platform::TargetTriple::systems
{
    { "none", TargetSystem::None },
    { "linux", TargetSystem::Linux },
    { "windows", TargetSystem::Windows },
    { "win32", TargetSystem::Windows },
    { "darwin", TargetSystem::Darwin },
    { "macos", TargetSystem::Darwin },
    { "ios", TargetSystem::Darwin },
};

const std::unordered_map<std::string_view, scc::platform::TargetEnvironment> scc::platform::TargetTriple::environments
{
    { "eabi", TargetEnvironment::EABI },
    { "gnu", TargetEnvironment::GNU },
    { "android", TargetEnvironment::Android },
    { "macho", TargetEnvironment::MachO },
    { "elf", TargetEnvironment::ELF },
};

const std::unordered_map<std::string_view, size_t> scc::platform::TargetTriple::bit_widths
{
    { "x86", 32 },
    { "i386", 32 },
    { "i486", 32 },
    { "i586", 32 },
    { "i686", 32 },
    { "x86_64", 64 },

    { "ppc", 32 },
    { "ppc32", 32 },
    { "ppc64", 64 },
    { "ppc64le", 64 },

    { "arm", 32 },
    { "armv7", 32 },
    { "thumb", 32 },
    { "aarch32", 32 },
    { "aarch64", 64 },

    { "riscv32", 32 },
    { "riscv64", 64 },
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

std::string scc::platform::TargetTriple::ToString() const
{
    return Architecture + '-' + System + '-' + Environment;
}

scc::Platform scc::platform::TargetTriple::GetPlatform(const TargetOptions &options) const
{
    return {
        .IR = GetIR(),
        .ISA = GetISA(),
        .ABI = GetABI(),
        .Features = {},
    };
}

scc::platform::TargetIR scc::platform::TargetTriple::GetIR() const
{
    switch (GetArchitecture())
    {
    case TargetArchitecture::X86:
        return {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64, 80 },
        };

    case TargetArchitecture::PPC:
        return {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64 },
        };

    case TargetArchitecture::ARM:
        return {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 16, 32, 64 },
        };

    case TargetArchitecture::RiscV:
        return {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64 },
        };
    }

    throw std::runtime_error("TODO");
}

scc::platform::TargetISA scc::platform::TargetTriple::GetISA() const
{
    TargetISA isa;

    switch (GetArchitecture())
    {
    case TargetArchitecture::X86:
        isa = {
            .Architecture = TargetArchitecture::X86,
            .Endianness = TargetEndianness::LittleEndian,
            .Registers = {
                // A
                { 0b0000, { .Code = 0b0000, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // C
                { 0b0001, { .Code = 0b0001, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // D
                { 0b0010, { .Code = 0b0010, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // B
                { 0b0011, { .Code = 0b0011, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // SP
                { 0b0100, { .Code = 0b0100, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // BP
                { 0b0101, { .Code = 0b0101, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // SI
                { 0b0110, { .Code = 0b0110, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // DI
                { 0b0111, { .Code = 0b0111, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R8
                { 0b1000, { .Code = 0b1000, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R9
                { 0b1001, { .Code = 0b1001, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R10
                { 0b1010, { .Code = 0b1010, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R11
                { 0b1011, { .Code = 0b1011, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R12
                { 0b1100, { .Code = 0b1100, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R13
                { 0b1101, { .Code = 0b1101, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R14
                { 0b1110, { .Code = 0b1110, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
                // R15
                { 0b1111, { .Code = 0b1111, .Class = TargetRegisterClass::GeneralPurpose, .Allocatable = true } },
            },
            .RegisterViews = {
                { Register::X86_AL, { .Code = 0b0000, .Name = "al", .BitWidth = 8 } },
                { Register::X86_AH, { .Code = 0b0000, .Name = "ah", .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_AX, { .Code = 0b0000, .Name = "ax", .BitWidth = 16 } },
                { Register::X86_EAX, { .Code = 0b0000, .Name = "eax", .BitWidth = 32 } },
                { Register::X86_RAX, { .Code = 0b0000, .Name = "rax", .BitWidth = 64 } },

                { Register::X86_CL, { .Code = 0b0001, .Name = "cl", .BitWidth = 8 } },
                { Register::X86_CH, { .Code = 0b0001, .Name = "ch", .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_CX, { .Code = 0b0001, .Name = "cx", .BitWidth = 16 } },
                { Register::X86_ECX, { .Code = 0b0001, .Name = "ecx", .BitWidth = 32 } },
                { Register::X86_RCX, { .Code = 0b0001, .Name = "rcx", .BitWidth = 64 } },

                { Register::X86_DL, { .Code = 0b0010, .Name = "dl", .BitWidth = 8 } },
                { Register::X86_DH, { .Code = 0b0010, .Name = "dh", .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_DX, { .Code = 0b0010, .Name = "dx", .BitWidth = 16 } },
                { Register::X86_EDX, { .Code = 0b0010, .Name = "edx", .BitWidth = 32 } },
                { Register::X86_RDX, { .Code = 0b0010, .Name = "rdx", .BitWidth = 64 } },

                { Register::X86_BL, { .Code = 0b0011, .Name = "bl", .BitWidth = 8 } },
                { Register::X86_BH, { .Code = 0b0011, .Name = "bh", .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_BX, { .Code = 0b0011, .Name = "bx", .BitWidth = 16 } },
                { Register::X86_EBX, { .Code = 0b0011, .Name = "ebx", .BitWidth = 32 } },
                { Register::X86_RBX, { .Code = 0b0011, .Name = "rbx", .BitWidth = 64 } },

                { Register::X86_SPL, { .Code = 0b0100, .Name = "spl", .BitWidth = 8 } },
                { Register::X86_SP, { .Code = 0b0100, .Name = "sp", .BitWidth = 16 } },
                { Register::X86_ESP, { .Code = 0b0100, .Name = "esp", .BitWidth = 32 } },
                { Register::X86_RSP, { .Code = 0b0100, .Name = "rsp", .BitWidth = 64 } },

                { Register::X86_BPL, { .Code = 0b0101, .Name = "bpl", .BitWidth = 8 } },
                { Register::X86_BP, { .Code = 0b0101, .Name = "bp", .BitWidth = 16 } },
                { Register::X86_EBP, { .Code = 0b0101, .Name = "ebp", .BitWidth = 32 } },
                { Register::X86_RBP, { .Code = 0b0101, .Name = "rbp", .BitWidth = 64 } },

                { Register::X86_SIL, { .Code = 0b0110, .Name = "sil", .BitWidth = 8 } },
                { Register::X86_SI, { .Code = 0b0110, .Name = "si", .BitWidth = 16 } },
                { Register::X86_ESI, { .Code = 0b0110, .Name = "esi", .BitWidth = 32 } },
                { Register::X86_RSI, { .Code = 0b0110, .Name = "rsi", .BitWidth = 64 } },

                { Register::X86_DIL, { .Code = 0b0111, .Name = "dil", .BitWidth = 8 } },
                { Register::X86_DI, { .Code = 0b0111, .Name = "di", .BitWidth = 16 } },
                { Register::X86_EDI, { .Code = 0b0111, .Name = "edi", .BitWidth = 32 } },
                { Register::X86_RDI, { .Code = 0b0111, .Name = "rdi", .BitWidth = 64 } },

                { Register::X86_R8B, { .Code = 0b1000, .Name = "r8b", .BitWidth = 8 } },
                { Register::X86_R8W, { .Code = 0b1000, .Name = "r8w", .BitWidth = 16 } },
                { Register::X86_R8D, { .Code = 0b1000, .Name = "r8d", .BitWidth = 32 } },
                { Register::X86_R8, { .Code = 0b1000, .Name = "r8", .BitWidth = 64 } },

                { Register::X86_R9B, { .Code = 0b1000, .Name = "r9b", .BitWidth = 8 } },
                { Register::X86_R9W, { .Code = 0b1000, .Name = "r9w", .BitWidth = 16 } },
                { Register::X86_R9D, { .Code = 0b1000, .Name = "r9d", .BitWidth = 32 } },
                { Register::X86_R9, { .Code = 0b1000, .Name = "r9", .BitWidth = 64 } },

                { Register::X86_R10B, { .Code = 0b1000, .Name = "r10b", .BitWidth = 8 } },
                { Register::X86_R10W, { .Code = 0b1000, .Name = "r10w", .BitWidth = 16 } },
                { Register::X86_R10D, { .Code = 0b1000, .Name = "r10d", .BitWidth = 32 } },
                { Register::X86_R10, { .Code = 0b1000, .Name = "r10", .BitWidth = 64 } },

                { Register::X86_R11B, { .Code = 0b1000, .Name = "r11b", .BitWidth = 8 } },
                { Register::X86_R11W, { .Code = 0b1000, .Name = "r11w", .BitWidth = 16 } },
                { Register::X86_R11D, { .Code = 0b1000, .Name = "r11d", .BitWidth = 32 } },
                { Register::X86_R11, { .Code = 0b1000, .Name = "r11", .BitWidth = 64 } },

                { Register::X86_R12B, { .Code = 0b1000, .Name = "r12b", .BitWidth = 8 } },
                { Register::X86_R12W, { .Code = 0b1000, .Name = "r12w", .BitWidth = 16 } },
                { Register::X86_R12D, { .Code = 0b1000, .Name = "r12d", .BitWidth = 32 } },
                { Register::X86_R12, { .Code = 0b1000, .Name = "r12", .BitWidth = 64 } },

                { Register::X86_R13B, { .Code = 0b1000, .Name = "r13b", .BitWidth = 8 } },
                { Register::X86_R13W, { .Code = 0b1000, .Name = "r13w", .BitWidth = 16 } },
                { Register::X86_R13D, { .Code = 0b1000, .Name = "r13d", .BitWidth = 32 } },
                { Register::X86_R13, { .Code = 0b1000, .Name = "r13", .BitWidth = 64 } },

                { Register::X86_R14B, { .Code = 0b1000, .Name = "r14b", .BitWidth = 8 } },
                { Register::X86_R14W, { .Code = 0b1000, .Name = "r14w", .BitWidth = 16 } },
                { Register::X86_R14D, { .Code = 0b1000, .Name = "r14d", .BitWidth = 32 } },
                { Register::X86_R14, { .Code = 0b1000, .Name = "r14", .BitWidth = 64 } },

                { Register::X86_R15B, { .Code = 0b1000, .Name = "r15b", .BitWidth = 8 } },
                { Register::X86_R15W, { .Code = 0b1000, .Name = "r15w", .BitWidth = 16 } },
                { Register::X86_R15D, { .Code = 0b1000, .Name = "r15d", .BitWidth = 32 } },
                { Register::X86_R15, { .Code = 0b1000, .Name = "r15", .BitWidth = 64 } },
            },
            .Mnemonics = {
                { Mnemonic::X86_MOV, { .Name = "mov" } },
                { Mnemonic::X86_ADD, { .Name = "add" } },
                { Mnemonic::X86_SUB, { .Name = "sub" } },
                { Mnemonic::X86_IMUL, { .Name = "imul" } },
                { Mnemonic::X86_DIV, { .Name = "div" } },
                { Mnemonic::X86_INC, { .Name = "inc" } },
                { Mnemonic::X86_DEC, { .Name = "dec" } },
                { Mnemonic::X86_XOR, { .Name = "xor" } },
                { Mnemonic::X86_CALL, { .Name = "call" } },
                { Mnemonic::X86_RET, { .Name = "ret" } },
                { Mnemonic::X86_PUSH, { .Name = "push" } },
                { Mnemonic::X86_POP, { .Name = "pop" } },
                { Mnemonic::X86_JMP, { .Name = "jmp" } },
                { Mnemonic::X86_CMP, { .Name = "cmp" } },
                { Mnemonic::X86_JEQ, { .Name = "jeq" } },
                { Mnemonic::X86_JNE, { .Name = "jne" } },
                { Mnemonic::X86_JLT, { .Name = "jlt" } },
                { Mnemonic::X86_JLE, { .Name = "jle" } },
                { Mnemonic::X86_JGT, { .Name = "jgt" } },
                { Mnemonic::X86_JGE, { .Name = "jge" } },
                { Mnemonic::X86_SYSCALL, { .Name = "syscall" } },
            },
        };
        break;

    case TargetArchitecture::PPC:
        isa = {
            .Architecture = TargetArchitecture::PPC,
            .Endianness = TargetEndianness::BigEndian,
            .Registers = {},
            .RegisterViews = {},
            .Mnemonics = {},
        };
        break;

    case TargetArchitecture::ARM:
        isa = {
            .Architecture = TargetArchitecture::ARM,
            .Endianness = TargetEndianness::LittleEndian,
            .Registers = {},
            .RegisterViews = {},
            .Mnemonics = {},
        };
        break;

    case TargetArchitecture::RiscV:
        isa = {
            .Architecture = TargetArchitecture::RiscV,
            .Endianness = TargetEndianness::LittleEndian,
            .Registers = {},
            .RegisterViews = {},
            .Mnemonics = {},
        };
        break;

    default:
        throw std::runtime_error("TODO");
    }

    for (auto &[fst, snd] : isa.RegisterViews)
        isa.RegisterNames[snd.Name] = fst;

    for (auto &[fst, snd] : isa.Mnemonics)
        isa.MnemonicNames[snd.Name] = fst;

    return isa;
}

scc::platform::TargetABI scc::platform::TargetTriple::GetABI() const
{
    switch (GetSystem())
    {
    case TargetSystem::None:
        return {
            .DataLayout = {
                .PointerSize = Is32Bit() ? 4u : 8u,
                .PointerAlign = Is32Bit() ? 4u : 8u,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
                .HasRedZone = false,
                .CallingConvention = GetArchitecture() == TargetArchitecture::ARM
                                         ? TargetCallingConvention::AAPCS
                                         : GetArchitecture() == TargetArchitecture::RiscV
                                         ? TargetCallingConvention::RiscVCall
                                         : TargetCallingConvention::SystemV,
            },
            .ObjectFormat = TargetObjectFormat::ELF,
        };

    case TargetSystem::Linux:
        return {
            .DataLayout = {
                .PointerSize = Is32Bit() ? 4u : 8u,
                .PointerAlign = Is32Bit() ? 4u : 8u,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
                .HasRedZone = GetArchitecture() == TargetArchitecture::X86,
                .CallingConvention = GetArchitecture() == TargetArchitecture::ARM
                                         ? TargetCallingConvention::AAPCS
                                         : GetArchitecture() == TargetArchitecture::RiscV
                                         ? TargetCallingConvention::RiscVCall
                                         : TargetCallingConvention::SystemV,
            },
            .ObjectFormat = TargetObjectFormat::ELF,
        };

    case TargetSystem::Windows:
        return {
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
        return {
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
    }

    throw std::runtime_error("TODO");
}

scc::platform::TargetArchitecture scc::platform::TargetTriple::GetArchitecture() const
{
    return architectures.at(Architecture);
}

scc::platform::TargetSystem scc::platform::TargetTriple::GetSystem() const
{
    return systems.at(System);
}

scc::platform::TargetEnvironment scc::platform::TargetTriple::GetEnvironment() const
{
    return environments.at(Environment);
}

size_t scc::platform::TargetTriple::GetBitWidth() const
{
    return bit_widths.at(Architecture);
}

bool scc::platform::TargetTriple::Is32Bit() const
{
    return GetBitWidth() == 32;
}

bool scc::platform::TargetTriple::Is64Bit() const
{
    return GetBitWidth() == 64;
}

scc::platform::TargetTriple scc::platform::ParseTriple(const std::string_view triple)
{
    const auto segments = split(triple);

    return {
        .Architecture = std::string(segments[0]),
        .System = std::string(segments[1]),
        .Environment = std::string(segments[2]),
    };
}

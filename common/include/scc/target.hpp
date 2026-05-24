#pragma once

#include <scc/common.hpp>

#include <toolkit/result.hxx>

#include <bitset>
#include <cstdint>
#include <format>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace scc::platform
{
    using TargetFeatures = std::bitset<256>;

    enum class TargetArchitecture
    {
        X86,
        PPC,
        ARM,
        RiscV,
    };

    enum class TargetSystem
    {
        None,
        Linux,
        Windows,
        Darwin,
    };

    enum class TargetEnvironment
    {
        EABI,
        GNU,
        MSVC,
        ELF,
        Android,
        MachO,
    };

    enum class TargetEndianness
    {
        LittleEndian,
        BigEndian,
    };

    enum class TargetCallingConvention
    {
        /**
         * cdecl
         * - arguments on stack, right-to-left
         * - result in %eax or %st0
         * - caller saves %eax, %ecx and %edx
         * - caller resets stack pointer
         */
        CDECL,

        /**
         * syscall
         * - arguments on stack, right-to-left
         * - result in %eax or %st0
         * - caller resets stack pointer
         * - argument count (in dwords) in %al
         */
        SYSCALL,

        /**
         * optlink
         * - arg0 in %eax or %st0, arg1 in %edx or %st1, arg2 in %ecx or %st2, arg3 in %st3, rest on stack, right-to-left
         * - floating-point arguments have reserved space on stack, even though they can be passed in registers
         * - result in %eax or %st0
         * - caller saves %ebp, %ebx, %esi and %edi
         * - caller resets stack pointer
         */
        OPTLINK,

        /**
         * pascal
         * - arguments on stack, left-to-right
         * - ordinal result in %al (8-bit), %ax (16-bit), %eax (32-bit) or %dx:%ax (2x16-bit)
         * - real result in %dx:%bx:%ax
         * - floating-point result in %st0
         * - pointer result in %ax (16-bit) or %eax (32-bit)
         * - string result at temporary address, pointed to by @Result
         * - callee resets stack pointer
         */
        PASCAL,

        /**
         * stdcall
         * - arguments on stack, right-to-left
         * - result in %eax
         * - callee resets stack pointer
         * - caller saves %eax, %ecx and %edx
         */
        STDCALL,

        /**
         * fastcall
         * - arg0 in %ecx, arg1 in %edx, rest on stack, right-to-left
         * - callee resets stack pointer
         */
        FASTCALL,

        /**
         * System-V
         * - arguments in:
         *   - %rdi, %rsi, %rdx, %rcx, %r8, %r9
         *   - static chain pointer in %r10
         *   - %xmm0 - %xmm7
         *   - rest on stack, right-to-left
         * - result in:
         *   - %rax (64-bit) or %rax:%rdx (128-bit)
         *   - %xmm0 and %xmm1
         */
        SystemV,

        /**
         * Microsoft x64
         */
        MSX64,

        /**
         * ARM Procedure Call Standard
         */
        AAPCS,

        /**
         * RiscV Call
         */
        RiscVCall,
    };

    enum class TargetObjectFormat
    {
        Raw,
        ELF,
        PE,
        COFF,
        MachO,
    };

    struct TargetDataLayout
    {
        size_t PointerSize{};
        size_t PointerAlign{};

        size_t MaxIntAlign{};
        size_t MaxFloatAlign{};
        size_t MaxVectorAlign{};
        size_t MaxAggregateAlign{};
    };

    struct TargetCallLayout
    {
        size_t StackAlign{};
        bool HasRedZone{};

        TargetCallingConvention CallingConvention{};
    };

    enum class RegisterClass
    {
        GeneralPurpose,
        FloatingPoint,
        Vector,
        Special,
    };

    struct RegisterDescriptor
    {
        uint32_t Code{};
        RegisterClass Class{};

        bool Allocatable{};
    };

    struct RegisterView
    {
        [[nodiscard]] std::string_view CanonicalName() const;

        uint32_t Code{};
        std::vector<std::string_view> Names;

        size_t BitWidth{};
        size_t BitOffset{};
    };

    enum class OperandKind
    {
        None,

        Register,
        Immediate,
        Memory,
        Relative,
    };

    enum class OperandClass
    {
        None,

        GeneralPurpose,
        FloatingPoint,
        Vector,
        Special,
    };

    struct OperandConstraint
    {
        OperandKind Kind{};
        OperandClass Class{};

        uint16_t MinBitWidth{};
        uint16_t MaxBitWidth{};

        bool Optional{};
    };

    enum class EncodingKind
    {
        Invalid,

        X86_ModRM,
        X86_OpcodePlusRegister,
        X86_Group,
        X86_Relative,
        X86_NoOperands,

        RISCV_R,
        RISCV_I,
        RISCV_S,
        RISCV_B,
        RISCV_U,
        RISCV_J,
    };

    struct InstructionForm
    {
        Mnemonic Name{};

        std::span<const OperandConstraint> Operands;

        EncodingKind Encoding{};
        uint32_t EncodingOffset{};

        TargetFeatures RequiredFeatures;
    };

    struct MnemonicDescriptor
    {
        [[nodiscard]] std::string_view CanonicalName() const;

        std::vector<std::string_view> Names;

        uint32_t FormBegin{};
        uint32_t FormEnd{};
    };

    struct IntermediateRepresentation
    {
        std::unordered_set<size_t> LegalIntWidth;
        std::unordered_set<size_t> LegalFloatWidth;
    };

    struct InstructionSetArchitecture
    {
        const RegisterView *FindRegisterView(Register key) const;
        const RegisterView *FindRegisterView(std::string_view key) const;
        std::optional<Register> FindRegisterName(std::string_view key) const;

        const MnemonicDescriptor *FindMnemonic(Mnemonic key) const;
        const MnemonicDescriptor *FindMnemonic(std::string_view key) const;
        std::optional<Mnemonic> FindMnemonicName(std::string_view key) const;

        TargetArchitecture Architecture{};
        TargetEndianness Endianness{};

        uint8_t BitWidth{};

        std::unordered_map<uint32_t, RegisterDescriptor> Registers;
        std::unordered_map<Register, RegisterView> RegisterViews;

        std::vector<InstructionForm> Forms;
        std::unordered_map<Mnemonic, MnemonicDescriptor> Mnemonics;

        std::unordered_map<std::string_view, Register> RegisterNames;
        std::unordered_map<std::string_view, Mnemonic> MnemonicNames;
    };

    struct ApplicationBinaryInterface
    {
        TargetDataLayout DataLayout;
        TargetCallLayout CallLayout;

        TargetObjectFormat ObjectFormat{};
    };

    struct Options
    {
        uint8_t BitWidth{};
    };

    /**
     * x86_64-linux-gnu, aarch64-linux-android, etc.
     *
     * {architecture}-{system}-{environment}
     */
    struct TargetTriple
    {
        [[nodiscard]] toolkit::result<Platform> GetPlatform() const;

        [[nodiscard]] toolkit::result<IntermediateRepresentation> GetIR() const;
        [[nodiscard]] toolkit::result<InstructionSetArchitecture> GetISA() const;
        [[nodiscard]] toolkit::result<ApplicationBinaryInterface> GetABI() const;

        std::string Name;

        TargetArchitecture Architecture{};
        TargetSystem System{};
        TargetEnvironment Environment{};

        uint8_t BitWidth{};
    };

    toolkit::result<TargetTriple> ParseTriple(std::string_view triple);

    extern const std::unordered_map<std::string_view, std::pair<TargetArchitecture, uint8_t>> architectures;
    extern const std::unordered_map<std::string_view, TargetSystem> systems;
    extern const std::unordered_map<std::string_view, TargetEnvironment> environments;
}

template<>
struct std::formatter<scc::platform::TargetArchitecture>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    auto format(const scc::platform::TargetArchitecture value, C &&ctx) const
    {
        static const std::unordered_map<scc::platform::TargetArchitecture, const char *> map
        {
            { scc::platform::TargetArchitecture::X86, "x86" },
            { scc::platform::TargetArchitecture::PPC, "ppc" },
            { scc::platform::TargetArchitecture::ARM, "arm" },
            { scc::platform::TargetArchitecture::RiscV, "riscv" },
        };

        if (const auto it = map.find(value); it != map.end())
            return std::format_to(ctx.out(), "{}", it->second);

        return ctx.out();
    }
};

template<>
struct std::formatter<scc::platform::TargetSystem>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    auto format(const scc::platform::TargetSystem value, C &&ctx) const
    {
        static const std::unordered_map<scc::platform::TargetSystem, const char *> map
        {
            { scc::platform::TargetSystem::None, "none" },
            { scc::platform::TargetSystem::Linux, "linux" },
            { scc::platform::TargetSystem::Windows, "windows" },
            { scc::platform::TargetSystem::Darwin, "darwin" },
        };

        if (const auto it = map.find(value); it != map.end())
            return std::format_to(ctx.out(), "{}", it->second);

        return ctx.out();
    }
};

template<>
struct std::formatter<scc::platform::TargetEnvironment>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    auto format(const scc::platform::TargetEnvironment value, C &&ctx) const
    {
        static const std::unordered_map<scc::platform::TargetEnvironment, const char *> map
        {
            { scc::platform::TargetEnvironment::EABI, "eabi" },
            { scc::platform::TargetEnvironment::GNU, "gnu" },
            { scc::platform::TargetEnvironment::MSVC, "msvc" },
            { scc::platform::TargetEnvironment::ELF, "elf" },
            { scc::platform::TargetEnvironment::Android, "android" },
            { scc::platform::TargetEnvironment::MachO, "macho" },
        };

        if (const auto it = map.find(value); it != map.end())
            return std::format_to(ctx.out(), "{}", it->second);

        return ctx.out();
    }
};

#pragma once

#include <bitset>
#include <scc/common.hpp>

#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <scc/platform.hpp>

namespace scc
{
    enum class PlatformArchitecture
    {
        X86,
        PPC,
        ARM,
        RISCV,
    };

    enum class PlatformEndianness
    {
        LittleEndian,
        BigEndian,
    };

    enum class PlatformCallingConvention
    {
        /**
         * cdecl
         * - arguments on stack, right-to-left
         * - result in %eax or %st0
         * - caller saves %eax, %ecx and %edx
         * - caller resets stack pointer
         */
        cdecl,

        /**
         * syscall
         * - arguments on stack, right-to-left
         * - result in %eax or %st0
         * - caller resets stack pointer
         * - argument count (in dwords) in %al
         */
        syscall,

        /**
         * optlink
         * - arg0 in %eax or %st0, arg1 in %edx or %st1, arg2 in %ecx or %st2, arg3 in %st3, rest on stack, right-to-left
         * - floating-point arguments have reserved space on stack, even though they can be passed in registers
         * - result in %eax or %st0
         * - caller saves %ebp, %ebx, %esi and %edi
         * - caller resets stack pointer
         */
        optlink,

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
        pascal,

        /**
         * stdcall
         * - arguments on stack, right-to-left
         * - result in %eax
         * - callee resets stack pointer
         * - caller saves %eax, %ecx and %edx
         */
        stdcall,

        /**
         * fastcall
         * - arg0 in %ecx, arg1 in %edx, rest on stack, right-to-left
         * - callee resets stack pointer
         */
        fastcall,

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
        systemv,

        /**
         * Microsoft x64
         */
        msx64,

        /**
         * ARM Procedure Call Standard
         */
        aapcs,

        /**
         * RiscV Call
         */
        rvcall,
    };

    struct PlatformIR
    {
        std::unordered_set<size_t> LegalIntWidth;
        std::unordered_set<size_t> LegalFloatWidth;
    };

    struct PlatformISA
    {
        PlatformArchitecture Architecture;
        PlatformEndianness Endianness;

        std::unordered_map<std::string_view, Mnemonic> Mnemonics;
        std::unordered_map<std::string_view, Register> Registers;
    };

    struct PlatformABI
    {
        size_t PointerSize{};
        size_t PointerAlign{};

        size_t StackAlign{};

        size_t MaxIntAlign{};
        size_t MaxFloatAlign{};
        size_t MaxVectorAlign{};
        size_t MaxAggregateAlign{};
    };

    enum class PlatformFeature : uint16_t
    {
    };

    using PlatformFeatures = std::bitset<256>;

    struct Platform
    {
        PlatformIR IR;
        PlatformISA ISA;
        PlatformABI ABI;
        PlatformFeatures Features;
    };

    template<PlatformArchitecture Architecture>
    struct PlatformOptions;

    template<PlatformArchitecture Architecture>
    Platform GetPlatform(const PlatformOptions<Architecture> &options) = delete;

    template<>
    struct PlatformOptions<PlatformArchitecture::X86>
    {
    };

    template<>
    Platform GetPlatform(const PlatformOptions<PlatformArchitecture::X86> &options);

    template<>
    struct PlatformOptions<PlatformArchitecture::PPC>
    {
    };

    template<>
    Platform GetPlatform(const PlatformOptions<PlatformArchitecture::PPC> &options);

    template<>
    struct PlatformOptions<PlatformArchitecture::ARM>
    {
    };

    template<>
    Platform GetPlatform(const PlatformOptions<PlatformArchitecture::ARM> &options);

    template<>
    struct PlatformOptions<PlatformArchitecture::RISCV>
    {
    };

    template<>
    Platform GetPlatform(const PlatformOptions<PlatformArchitecture::RISCV> &options);
}

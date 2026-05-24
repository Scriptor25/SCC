#pragma once

namespace scc
{
    enum class Mnemonic
    {
#pragma region X86

        /* 8086 */

        X86_AAA,
        X86_AAD,
        X86_AAM,
        X86_AAS,
        X86_ADC,
        X86_ADD,
        X86_AND,
        X86_CALL,
        X86_CBW,
        X86_CLC,
        X86_CLD,
        X86_CLI,
        X86_CMC,
        X86_CMP,
        X86_CMPSB,
        X86_CMPSW,
        X86_CWD,
        X86_DAA,
        X86_DAS,
        X86_DEC,
        X86_DIV,
        X86_ESC,
        X86_HLT,
        X86_IDIV,
        X86_IMUL,
        X86_IN,
        X86_INC,
        X86_INT,
        X86_INTO,
        X86_IRET,

        X86_J_,
        X86_JO = X86_J_,
        X86_JNO,
        X86_JC,
        X86_JB   = X86_JC,
        X86_JNAE = X86_JC,
        X86_JNC,
        X86_JNB = X86_JNC,
        X86_JAE = X86_JNC,
        X86_JZ,
        X86_JE = X86_JZ,
        X86_JNZ,
        X86_JNE = X86_JNZ,
        X86_JNA,
        X86_JBE = X86_JNA,
        X86_JA,
        X86_JNBE = X86_JA,
        X86_JS,
        X86_JNS,
        X86_JP,
        X86_JPE = X86_JP,
        X86_JNP,
        X86_JPO = X86_JNP,
        X86_JL,
        X86_JNGE = X86_JL,
        X86_JNL,
        X86_JGE = X86_JNL,
        X86_JLE,
        X86_JNG = X86_JLE,
        X86_JNLE,
        X86_JG = X86_JNLE,

        X86_JCXZ,
        X86_JECXZ = X86_JCXZ,

        X86_JMP,
        X86_LAHF,
        X86_LDS,
        X86_LEA,
        X86_LES,
        X86_LOCK,
        X86_LODSB,
        X86_LODSW,

        X86_LOOP,
        X86_LOOPE,
        X86_LOOPNE,
        X86_LOOPNZ,
        X86_LOOPZ,

        X86_MOV,
        X86_MOVSB,
        X86_MOVSW,
        X86_MUL,
        X86_NEG,
        X86_NOP,
        X86_NOT,
        X86_OR,
        X86_OUT,
        X86_POP,
        X86_POPF,
        X86_PUSH,
        X86_PUSHF,
        X86_RCL,
        X86_RCR,

        X86_REP,
        X86_REPE,
        X86_REPNE,
        X86_REPNZ,
        X86_REPZ,

        X86_RET,
        X86_RETN,
        X86_RETF,
        X86_ROL,
        X86_ROR,
        X86_SAHF,
        X86_SAL,
        X86_SAR,
        X86_SBB,
        X86_SCASB,
        X86_SCASW,
        X86_SHL,
        X86_SHR,
        X86_STC,
        X86_STD,
        X86_STI,
        X86_STOSB,
        X86_STOSW,
        X86_SUB,
        X86_TEST,
        X86_WAIT,
        X86_XCHG,
        X86_XLAT,
        X86_XOR,

        /* 80186 */

        X86_PUSHA,
        X86_POPA,
        X86_BOUND,
        X86_IIMUL,
        X86_INSB,
        X86_INSW,
        X86_OUTSB,
        X86_OUTSW,
        X86_ENTER,
        X86_LEAVE,

        /* 80286 */

        X86_LGDT,
        X86_LIDT,
        X86_LMSW,
        X86_CLTS,
        X86_LLDT,
        X86_LTR,
        X86_SGDT,
        X86_SIDT,
        X86_SMSW,
        X86_SLDT,
        X86_STR,
        X86_ARPL,
        X86_LAR,
        X86_LSL,
        X86_VERR,
        X86_VERW,
        X86_LOADALL,
        X86_STOREALL,

        /* 80386 */

        X86_LODSD,
        X86_STOSD,
        X86_MOVSD,
        X86_CMPSD,
        X86_SCASD,
        X86_INSD,
        X86_OUTSD,
        X86_CWDE,
        X86_CDQ,
        X86_PUSHAD,
        X86_POPAD,
        X86_PUSHFD,
        X86_POPFD,
        X86_IRETD,
        X86_BT,
        X86_BTS,
        X86_BTR,
        X86_BTC,
        X86_BSF,
        X86_BSR,
        X86_SHLD,
        X86_SHRD,
        X86_MOVZX,
        X86_MOVSX,

        X86_SET_,
        X86_SETO = X86_SET_,
        X86_SETNO,
        X86_SETC,
        X86_SETB   = X86_SETC,
        X86_SETNAE = X86_SETC,
        X86_SETNC,
        X86_SETNB = X86_SETNC,
        X86_SETAE = X86_SETNC,
        X86_SETZ,
        X86_SETE = X86_SETZ,
        X86_SETNZ,
        X86_SETNE = X86_SETNZ,
        X86_SETNA,
        X86_SETBE = X86_SETNA,
        X86_SETA,
        X86_SETNBE = X86_SETA,
        X86_SETS,
        X86_SETNS,
        X86_SETP,
        X86_SETPE = X86_SETP,
        X86_SETNP,
        X86_SETPO = X86_SETNP,
        X86_SETL,
        X86_SETNGE = X86_SETL,
        X86_SETNL,
        X86_SETGE = X86_SETNL,
        X86_SETLE,
        X86_SETNG = X86_SETLE,
        X86_SETNLE,
        X86_SETG = X86_SETNLE,

        X86_LFS,
        X86_LGS,
        X86_LSS,
        X86_ICEBP,
        X86_INT01,
        X86_INT1,
        X86_UMOV,
        X86_XBTS,
        X86_IBTS,
        X86_LOADALLD,
        X86_LOADALL386,

        /* 80486 */

        X86_BSWAP,
        X86_CMPXCHG,
        X86_XADD,
        X86_INVLPG,
        X86_WBINVD,
        X86_INVD,

        /* P5/P6 */

        X86_RDMSR,
        X86_WRMSR,
        X86_RSM,
        X86_CPUID,
        X86_CMPXCHG8B,
        X86_RDTSC,
        X86_RDPMC,

        X86_CMOV_,
        X86_CMOVO = X86_CMOV_,
        X86_CMOVNO,
        X86_CMOVC,
        X86_CMOVB   = X86_CMOVC,
        X86_CMOVNAE = X86_CMOVC,
        X86_CMOVNC,
        X86_CMOVNB = X86_CMOVNC,
        X86_CMOVAE = X86_CMOVNC,
        X86_CMOVZ,
        X86_CMOVE = X86_CMOVZ,
        X86_CMOVNZ,
        X86_CMOVNE = X86_CMOVNZ,
        X86_CMOVNA,
        X86_CMOVBE = X86_CMOVNA,
        X86_CMOVA,
        X86_CMOVNBE = X86_CMOVA,
        X86_CMOVS,
        X86_CMOVNS,
        X86_CMOVP,
        X86_CMOVPE = X86_CMOVP,
        X86_CMOVNP,
        X86_CMOVPO = X86_CMOVNP,
        X86_CMOVL,
        X86_CMOVNGE = X86_CMOVL,
        X86_CMOVNL,
        X86_CMOVGE = X86_CMOVNL,
        X86_CMOVLE,
        X86_CMOVNG = X86_CMOVLE,
        X86_CMOVNLE,
        X86_CMOVG = X86_CMOVNLE,

        X86_NOPL,
        X86_UD2,
        X86_UD2A = X86_UD2,
        X86_UD1,
        X86_UD2B = X86_UD1,
        X86_UD0,
        X86_OIO = X86_UD0,
        X86_SYSCALL,
        X86_SYSRET,
        X86_SYSENTER,
        X86_SYSEXIT,

        /* x64 */

        X86_CDQE,
        X86_CQO,
        X86_CMPSQ,
        X86_CMPXCHG16B,
        X86_IRETQ,
        X86_JRCXZ,
        X86_LODSQ,
        X86_MOVSXD,
        X86_MOVSQ,
        X86_POPFQ,
        X86_PUSHFQ,
        X86_SCASQ,
        X86_STOSQ,
        X86_SWAPGS,
        X86_UDB,

        /* VEX */

        X86_POPCNT,
        X86_LZCNT,
        X86_TZCNT,
        X86_ANDN,
        X86_BEXTR,
        X86_BLSI,
        X86_BLSMSK,
        X86_BLSR,
        X86_BZHI,
        X86_MULX,
        X86_PDEP,
        X86_PEXT,
        X86_RORX,
        X86_SARX,
        X86_SHRX,
        X86_SHLX,

        /* TSX */

        X86_XBEGIN,
        X86_XABORT,
        X86_XEND,
        X86_XTEST,
        X86_XACQUIRE,
        X86_XRELEASE,
        X86_XSUSLDTRK,
        X86_XRESLDTRK,

        /* CET */

        X86_INCSSPD,
        X86_INCSSPQ,
        X86_RDSSPD,
        X86_RDSSPQ,
        X86_SAVEPREVSSP,
        X86_RSTORSSP,
        X86_WRSSD,
        X86_WRSSQ,
        X86_WRUSSD,
        X86_WRUSSQ,
        X86_SETSSBSY,
        X86_CLRSSBSY,
        X86_ENDBR32,
        X86_ENDBR64,
        X86_NOTRACK,

        /* XSAVE */

        X86_XSAVE,
        X86_XSAVE64,
        X86_XRSTOR,
        X86_XRSTOR64,
        X86_XGETBV,
        X86_XSETBV,
        X86_XSAVEOPT,
        X86_XSAVEOPT64,
        X86_XSAVEC,
        X86_XSAVEC64,
        X86_XSAVES,
        X86_XSAVES64,
        X86_XRSTORS,
        X86_XRSTORS64,

        /* SSE */

        X86_PREFETCHNTA,
        X86_PREFETCHT0,
        X86_PREFETCHT1,
        X86_PREFETCHT2,
        X86_SFENCE,

        /* SSE2 */

        X86_LFENCE,
        X86_MFENCE,
        X86_MOVNTI,
        X86_PAUSE,
        X86_HWNT,
        X86_HST,

        /* CLFSH */

        X86_CLFLUSH,

        /* MONITOR */

        X86_MONITOR,
        X86_MWAIT,

        /* SMX */

        X86_GETSEC,

        /* RDTSCP */

        X86_RDTSCP,

        /* SSE4.2 */

        X86_CRC32,

        /* FSGSBASE */

        X86_RDFSBASE,
        X86_RDGSBASE,
        X86_WRFSBASE,
        X86_WRGSBASE,

        /* MOVBE */

        X86_MOVBE,

        /* INVPCID */

        X86_INVPCID,

        /* PREFETCHW */

        X86_PREFETCHW,
        X86_PREFETCH,

        /* ADX */

        X86_ADCX,
        X86_ADOX,

        /* SMAP */

        X86_CLAC,
        X86_STAC,

        /* CLFLUSHOPT */

        X86_CLFLUSHOPT,

        /* PREFETCHWT1 */

        X86_PREFETCHWT1,

        /* PKU */

        X86_RDPKRU,
        X86_WRPKRU,

        /* CLWB */

        X86_CLWB,

        /* RDPID */

        X86_RDPID,

        /* MOVDIRI */

        X86_MOVDIRI,

        /* MOVDIR64B */

        X86_MOVDIR64B,

        /* WBNOINVD */

        X86_WBNOINVD,

        /* PREFETCHI */

        X86_PREFETCHIT0,
        X86_PREFETCHIT1,

        /* SGX */

        X86_ENCLS,
        X86_ENCLU,
        X86_ENCLV,

        /* PTWRITE */

        X86_PTWRITE,

        /* PCONFIG */

        X86_PCONFIG,

        /* CLDEMOTE */

        X86_CLDEMOTE,

        /* WAITPKG */

        X86_UMONITOR,
        X86_UMWAIT,
        X86_TPAUSE,

        /* SERIALIZE */

        X86_SERIALIZE,

        /* HRESET */

        X86_HRESET,

        /* IBHF */

        X86_IBHF,

        /* UINTR */

        X86_SENDUIPI,
        X86_UIRET,
        X86_TESTUI,
        X86_CLUI,
        X86_STUI,

        /* ENQCMD */

        X86_ENQCMD,
        X86_ENQCMDS,

        /* WRMSRNS */

        X86_WRMSRNS,

        /* MSRLIST */

        X86_RDMSRLIST,
        X86_WRMSRLIST,

        /* CMPccXADD */

        X86_CMDccXADD,

        /* PBNDKB */

        X86_PBNDKB,

        /* FRED */

        X86_ERETS,
        X86_ERETU,
        X86_LKGS,

        /* MONITORX */

        X86_MONITORX,
        X86_MWAITX,

        /* CLZERO */

        X86_CLZERO,

        /* RDPRU */

        X86_RDPRU,

        /* MCOMMIT */

        X86_MCOMMIT,

        /* INVLPGB */

        X86_INVLPGB,
        X86_TLBSYNC,

        /* 8087 */

        X86_FNINIT,
        X86_FLDCW,
        X86_FNSTCW,
        X86_FNSTSW,
        X86_FNCLEX,
        X86_FLDENV,
        X86_FNSTENV,
        X86_FNSAVE,
        X86_FRSTOR,
        X86_FNENI,
        X86_FNDISI,
        X86_FLD,
        X86_FST,
        X86_FSTP,
        X86_FLDZ,
        X86_FLD1,
        X86_FLDPI,
        X86_FLDL2T,
        X86_FLDL2E,
        X86_FLDLG2,
        X86_FLDLN2,
        X86_FXCH,
        X86_FILD,
        X86_FIST,
        X86_FISTP,
        X86_FBLD,
        X86_FBSTP,
        X86_FADD,
        X86_FMUL,
        X86_FSUB,
        X86_FSUBR,
        X86_FDIV,
        X86_FDIVR,
        X86_FCOM,
        X86_FADDP,
        X86_FMULP,
        X86_FSUBP,
        X86_FSUBRP,
        X86_FDIVP,
        X86_FDIVRP,
        X86_FCOMP,
        X86_FCOMPP,
        X86_FIADD,
        X86_FIMUL,
        X86_FISUB,
        X86_FISUBR,
        X86_FIDIV,
        X86_FIDIVR,
        X86_FICOM,
        X86_FICOMP,
        X86_FCHS,
        X86_FABS,
        X86_FTST,
        X86_FXAM,
        X86_FXTRACT,
        X86_FPREM,
        X86_FSQRT,
        X86_FRNDINT,
        X86_FSCALE,
        X86_F2XM1,
        X86_FYL2X,
        X86_FPTAN,
        X86_FPATAN,
        X86_FYL2XP1,
        X86_FNOP,
        X86_FDECSTP,
        X86_FINCSTP,
        X86_FFREE,
        X86_FWAIT,
        X86_FSTPNCE,
        X86_FFREEP,
        X86_FNSETPM,
        X86_FUCOM,
        X86_FUCOMP,
        X86_FUCOMPP,
        X86_FPREM1,
        X86_FSINCOS,
        X86_FSIN,
        X86_FCOS,
        X86_FCMOVB,
        X86_FCMOVE,
        X86_FCMOVBE,
        X86_FCMOVU,
        X86_FCMOVNB,
        X86_FCMOVNE,
        X86_FCMOVNBE,
        X86_FCMOVNU,
        X86_FCOMI,
        X86_FCOMIP,
        X86_FUCOMI,
        X86_FUCOMIP,
        X86_FXSAVE,
        X86_FXRSTOR,
        X86_FISTTP,

#pragma endregion

#pragma region RISCV

        RISCV_AND,
        RISCV_OR,
        RISCV_XOR,

        RISCV_ANDI,
        RISCV_ORI,
        RISCV_XORI,

        RISCV_SLL,
        RISCV_SRL,
        RISCV_SRA,

        RISCV_SLLI,
        RISCV_SRLI,
        RISCV_SRAI,

        RISCV_ADD,
        RISCV_SUB,
        RISCV_ADDI,
        RISCV_MUL,
        RISCV_DIV,
        RISCV_DIVU,
        RISCV_REM,
        RISCV_REMU,

        RISCV_LW,
        RISCV_LH,
        RISCV_LHU,
        RISCV_LB,
        RISCV_LBU,

        RISCV_SW,
        RISCV_SH,
        RISCV_SB,

        RISCV_BEQ,
        RISCV_BNE,
        RISCV_BLT,
        RISCV_BLTU,
        RISCV_BGE,
        RISCV_BGEU,

        RISCV_JAL,
        RISCV_JALR,

        RISCV_ECALL,
        RISCV_MRET,

        RISCV_CSRRW,

        RISCV_SLT,
        RISCV_SLTU,

        RISCV_SLTI,
        RISCV_SLTUI,

#pragma endregion
    };
}

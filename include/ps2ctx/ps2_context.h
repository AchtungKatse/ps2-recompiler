#pragma once
// #include "Threading/Semaphor.h"
// #include "Types.h"
// #include "Threading/SemaParam.h"
#include "ps2ctx/threading/semaphor.h"
#include "ps2rc/defines.h"

// #include <immintrin.h>
// #include <stdio.h>
// #include <threads.h>

#define KB 1024
#define MB 1024 * KB
#define ee_semaphore_count 256

typedef struct cop0_status {
    u32 InterruptsEnabled : 1;
    u32 ExceptionLevel : 1;
    u32 ErrorLevel : 1;
    u32 PrivilegeLevel : 2; // 0 = kernel, 1 = supervisior, 2 = user
    u32 INT0Enabled : 1;
    u32 INT1Enabeld : 1;
    u32 BusErrorMask : 1;
    u32 INT5Enable : 1;
    u32 MasterInterruptEnable : 1;
    u32 EDI : 1;
    u32 CacheHit : 1; // 0 = hit, 1 = miss
    u32 BEV : 1;
    u32 DEV : 1;
    u32 padding : 5; 
    u32 COPUsability : 4;
} COP0_STATUS;

typedef struct
{
    union
    {
        // Bit count
        u128 U128;
        s128 S128;
        u8 U8[16];
        s8 S8[16];
        u16 U16[8];
        s16 S16[8];
        u32 U32[4];
        s32 S32[4];
        u64 U64[2];
        s64 S64[2];

        // Name
        /*u128 U128;*/
        /*s128 S128;*/
        u8 UC[16];
        s8 SC[16];
        u16 US[8];
        s16 SS[8];
        u32 UL[4];
        s32 SL[4];
        u64 UD[2];
        s64 SD[2];
    };
} EEReg;

enum EERegister
{
    EE_zero,
    EE_at,
    EE_v0, EE_v1,
    EE_a0, EE_a1, EE_a2, EE_a3,
    EE_t0, EE_t1, EE_t2, EE_t3, EE_t4, EE_t5, EE_t6, EE_t7,
    EE_s0, EE_s1, EE_s2, EE_s3, EE_s4, EE_s5, EE_s6, EE_s7,
    EE_t8, EE_t9,
    EE_k0, EE_k1,
    EE_gp, EE_sp, EE_fp, EE_ra
};

enum FPURegister
{
    EE_f0, EE_f1, EE_f2, EE_f3, 
    EE_f4, EE_f5, EE_f6, EE_f7, EE_f8, EE_f9, EE_f10, EE_f11, 
    EE_f12, EE_f13, EE_f14, EE_f15, EE_f16, EE_f17, EE_f18, EE_f19, 
    EE_f20, EE_f21, EE_f22, EE_f23, EE_f24, EE_f25, EE_f26, EE_f27, EE_f28, EE_f29, EE_f30, EE_f31
};

typedef struct {
    // Config
    void* mainRAM;
    void* ioRegisterRAM;
    void* vu0CodeRAM;
    void* vu0DataRAM;
    void* vu1CodeRAM;
    void* vu1DataRAM;
    void* gsPrivilegedRAM;
    void* iopRAM;
} PS2Memory;


typedef struct
{
    // Emotion Engine
    union {
        struct { EEReg HI, LO, HI1, LO1;};
        struct { EEReg hi, lo, hi1, lo1; };
    };
    union { EEReg SA, sa; };
    union{
        EEReg ee_registers[32];
        EEReg GPR[32];
        struct {
            const EEReg zero;
            EEReg at,
                  v0, v1,
                  a0, a1, a2, a3,
                  t0, t1, t2, t3, t4, t5, t6, t7,
                  s0, s1, s2, s3, s4, s5, s6, s7,
                  t8, t9,
                  k0, k1,
                  gp, sp, fp, ra;
        };
    };

    // COP0
    u32 Index;
    u32 Random;
    u32 EntryLo0;
    u32 EntryLo1;
    u32 Context;
    u32 PageMask;
    u32 Wired;
    u32 BadVaddr;
    u32 Count;
    u32 EntryHi;
    u32 Compare;
    u64 Status;
    u32 Cause;
    u32 EPC;
    u32 PRid;
    u32 Config;
    union {u32 BadPaddr; u32 Reserved23; }; // Rabbitizer reads it as Reserved23 and not BadPaddr
    u32 Debug;
    u32 Perf;
    u32 TagLo;
    u32 TagHi;
    u32 ErrorEPC;

    // FPR
    union {
        float FPURegisters[32];
        struct {
            float f0, f1, f2, f3, // Return values
                  f4, f5, f6, f7, f8, f9, f10, f11, // Temporary Registers
                  f12, f13, f14, f15, f16, f17, f18, f19, // Argument Registers
                  f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31; // Saved registers
        };
    };

    u32 fcr0;
    u32 fcr31;  

    // VU0f
    // So some instructions want to use vf4.x
    // So basically there needs to be a union for the 256 bit vector units (which are technically 128 bit for the ps2 but it works)
    // Probably not going to work too well though considering the padding
    // Also I'm fairly sure this is linux only ATM
    union {  struct { float vf0x, vf0y, vf0z, vf0w; }; };
    union {  struct { float vf1x, vf1y, vf1z, vf1w; }; };
    union {  struct { float vf2x, vf2y, vf2z, vf2w; }; };
    union {  struct { float vf3x, vf3y, vf3z, vf3w; }; };
    union {  struct { float vf4x, vf4y, vf4z, vf4w; }; };
    union {  struct { float vf5x, vf5y, vf5z, vf5w; }; };
    union {  struct { float vf6x, vf6y, vf6z, vf6w; }; };
    union {  struct { float vf7x, vf7y, vf7z, vf7w; }; };
    union {  struct { float vf8x, vf8y, vf8z, vf8w; }; };
    union {  struct { float vf9x, vf9y, vf9z, vf9w; }; };
    union {  struct { float vf10x, vf10y, vf10z, vf10w; }; };
    union {  struct { float vf11x, vf11y, vf11z, vf11w; }; };
    union {  struct { float vf12x, vf12y, vf12z, vf12w; }; };
    union {  struct { float vf13x, vf13y, vf13z, vf13w; }; };
    union {  struct { float vf14x, vf14y, vf14z, vf14w; }; };
    union {  struct { float vf15x, vf15y, vf15z, vf15w; }; };
    union {  struct { float vf16x, vf16y, vf16z, vf16w; }; };
    union {  struct { float vf17x, vf17y, vf17z, vf17w; }; };
    union {  struct { float vf18x, vf18y, vf18z, vf18w; }; };
    union {  struct { float vf19x, vf19y, vf19z, vf19w; }; };
    union {  struct { float vf20x, vf20y, vf20z, vf20w; }; };
    union {  struct { float vf21x, vf21y, vf21z, vf21w; }; };
    union {  struct { float vf22x, vf22y, vf22z, vf22w; }; };
    union {  struct { float vf23x, vf23y, vf23z, vf23w; }; };
    union {  struct { float vf24x, vf24y, vf24z, vf24w; }; };
    union {  struct { float vf25x, vf25y, vf25z, vf25w; }; };
    union {  struct { float vf26x, vf26y, vf26z, vf26w; }; };
    union {  struct { float vf27x, vf27y, vf27z, vf27w; }; };
    union {  struct { float vf28x, vf28y, vf28z, vf28w; }; };
    union {  struct { float vf29x, vf29y, vf29z, vf29w; }; };
    union {  struct { float vf30x, vf30y, vf30z, vf30w; }; };
    union {  struct { float vf31x, vf31y, vf31z, vf31w; }; };
    union {   struct { float ACCx,  ACCy,  ACCz,  ACCw;  }; };

    // VU0i
    u16 vi0, vi1, vi2, vi3, vi4, vi5, vi6, vi7, vi8, vi9,
        vi10, vi11, vi12, vi13, vi14, vi15;

    union {u16 viStatus, vi16;}; // More unions because rabbitizer reads the special registers as viX
    union {u16 MACflag, vi17;};
    union {u16 ClipFlag, vi18;};
    union {u16 c2c19, vi19;};
    union {u16 R, vi20;};
    union {u16 I, vi21;};
    union {u16 Q, vi22;};
    union {u16 c2c23, vi23;};
    union {u16 c2c24, vi24;};
    union {u16 c2c25, vi25;};
    union {u16 TPC, vi26;};
    union {u16 CMSAR0, vi27;};
    union {u16 FBRST, vi28;};
    union {u16 VPU_STAT, vi29;};
    union {u16 c2c30, vi30;};
    union {u16 CMSAR1, vi31;};

    ee_semaphor_t ee_semaphors[ee_semaphore_count];
} RecompilationContext;

void* FindRam(u64 address);

extern EEReg ZeroRegister;
#define RecompilationContext_GetRegister(reg) (reg == 0 ? ZeroRegister : ctx->ee_registers[reg])

/* thread_local */ extern RecompilationContext* ctx;
extern PS2Memory* mem;
/* thread_local */ extern int EEThreadID;


#define __fi inline

#define Assert(cond, ...) { if (!(cond)) { printf(__VA_ARGS__); exit(-1); } }
#define LogDebug(...) printf("Debug: " __VA_ARGS__);

void jalr(u32 address);

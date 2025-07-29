#ifndef ASM_MACROS
#define ASM_MACROS
#include "ps2rc/defines.h"
#include "ps2ctx/ps2_context.h"

// Custom

#define MemorySize 0x2000000

void* FindRam(u64 address);
static u32 CountLeadingSignBits(s32 n);

#define assert(case) if (!case) { perror("Assert"); abort(); } 
#define __mips_asm_nop

// ===================================
// Normal EE Instructions
// ===================================
// 000
// *1
// *2
// j
#define __mips_asm__jal(function, nextInstruction) /* next_instr */ nextInstruction; /* function call */ function;
#define __mips_asm_jal(...) __mips_asm__jal(__VA_ARGS__)

#define __mips_asm_beq(rs, rt, branch, nextInstruction, i)       if (RecompilationContext_GetRegister(rs).S32[0] == RecompilationContext_GetRegister(rt).S32[0]) { nextInstruction; goto branch; } branch_##i: nextInstruction;
#define __mips_asm_bne(rs, rt, branch, nextInstruction, i)       if (RecompilationContext_GetRegister(rs).S32[0] != RecompilationContext_GetRegister(rt).S32[0]) { nextInstruction; goto branch; } branch_##i: nextInstruction;
#define __mips_asm_blez(rs, branch, nextInstruction, i)          if (RecompilationContext_GetRegister(rs).S32[0] <= 0) { nextInstruction; goto branch; } branch_##i: nextInstruction;
#define __mips_asm_bgtz(rs, branch, nextInstruction, i)          if (RecompilationContext_GetRegister(rs).S32[0] > 0) { nextInstruction; goto branch; } branch_##i: nextInstruction;

// 001
#define __mips_asm_addi(rs, rt, offset)        RecompilationContext_GetRegister(rs).U32[0] = (RecompilationContext_GetRegister(rt).U32[0] + offset);
#define __mips_asm_addiu(rs, rt, value)        RecompilationContext_GetRegister(rs).U32[0] = RecompilationContext_GetRegister(rt).U32[0] + value;
#define __mips_asm_slti(rd, rs, imm)           RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0] < imm;
#define __mips_asm_sltiu(rd, rs, imm)          RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0] < imm;
#define __mips_asm_andi(rd, rs, value)         RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0] & value;
#define __mips_asm_ori(rt, rs, immediate)      RecompilationContext_GetRegister(rt).U32[0] = RecompilationContext_GetRegister(rs).U32[0] | immediate;
#define __mips_asm_xori(rd, rs, imm)           RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0] ^ imm;
#define __mips_asm_lui(rd, value)              RecompilationContext_GetRegister(rd).U32[0] = value << 0x10;

// 010
#define __mips_asm_beql(rs, rt, branch, nextInstruction, i)  if (RecompilationContext_GetRegister(rs).U32[0] == RecompilationContext_GetRegister(rt).U32[0]) { nextInstruction; goto branch; } if (false) { branch_##i: nextInstruction;}
#define __mips_asm_bnel(rs, rt, branch, nextInstruction, i)  if (RecompilationContext_GetRegister(rs).U32[0] != RecompilationContext_GetRegister(rt).U32[0]) { nextInstruction; goto branch; } if (false) { branch_##i: nextInstruction; }

#define __mips_asm_blezl(rs, branch, nextInstruction, i)    __mips_asm_blez(rs, branch, nextInstruction, i)
#define __mips_asm_bgtzl(rs, branch, nextInstruction, i)    __mips_asm_bgtz(rs, branch, nextInstruction, i);

// 011
#define __mips_asm_daddi(rd, rs, imm)      RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rs).S64[0] + imm;
#define __mips_asm_daddiu(rd, rs, imm)     RecompilationContext_GetRegister(rd).U64[0] = RecompilationContext_GetRegister(rs).U64[0] + imm;
#define __mips_asm_ldl(rt, offset, base)   { u64 mask = (1 << (((RecompilationContext_GetRegister(base).U32[0] + offset - 0x7) % 8) * 8)) - 1; RecompilationContext_GetRegister(rt).U64[0] = (RecompilationContext_GetRegister(rt).U64[0] & ~mask) | (*(u64*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset - 0x7) & mask); }
#define __mips_asm_ldr(rt, offset, base)   { u64 mask = ~((1 << (((RecompilationContext_GetRegister(base).U32[0] + offset) % 8) * 8)) - 1); RecompilationContext_GetRegister(rt).U64[0] = (RecompilationContext_GetRegister(rt).U64[0] & ~mask) | (*(u64*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset) & mask); }
#define __mips_asm_lq(rt, offset, base)    ctx->ee_registers[rt].U128 = *(u128*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_sq(rt, offset, base)    memcpy(FindRam(RecompilationContext_GetRegister(base).U32[0] + offset), &ctx->ee_registers[rt].S128, sizeof(u128));

// 100
#define __mips_asm_lb(rt, offset, base)    RecompilationContext_GetRegister(rt).S8[0] = *(s8*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_lh(rt, offset, base)    RecompilationContext_GetRegister(rt).S16[0] = *(s16*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_lwl(rt, offset, base)   RecompilationContext_GetRegister(rt).S32[0] = *(s32*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_lw(rt, offset, base)    RecompilationContext_GetRegister(rt).S32[0] = *(s32*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_lbu(rt, offset, base)   RecompilationContext_GetRegister(rt).U8[0] = *(u8*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_lhu(rt, offset, base)   RecompilationContext_GetRegister(rt).U32[0] = *(u16*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_lwr(rt, offset, base)   RecompilationContext_GetRegister(rt).U32[0] = *(s32*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);
#define __mips_asm_lwu(rt, offset, base)   RecompilationContext_GetRegister(rt).U32[0] = *(u32*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);

// 101
#define __mips_asm_sb(rt, offset, base)    *(u8*)FindRam(offset + RecompilationContext_GetRegister(base).U32[0]) = RecompilationContext_GetRegister(rt).U8[0];
#define __mips_asm_sh(rt, offset, base)    *(s16*)FindRam(offset + RecompilationContext_GetRegister(base).U32[0]) = RecompilationContext_GetRegister(rt).U16[0];
#define __mips_asm_swl(rt, offset, base)   *(s32*)FindRam(offset + RecompilationContext_GetRegister(base).U32[0]) = RecompilationContext_GetRegister(rt).U32[0];
#define __mips_asm_sw(rt, offset, base)    *(s32*)FindRam(offset + RecompilationContext_GetRegister(base).U32[0]) = RecompilationContext_GetRegister(rt).U32[0];
#define __mips_asm_sdl(rt, offset, base)   { u64 mask = (1 << (((RecompilationContext_GetRegister(base).U64[0] + offset - 0x7) % 8) * 8)) - 1; u64 value = *(long*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset - 0x7); *(long*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset - 0x7) = (value & ~mask) | (RecompilationContext_GetRegister(rt).U64[0] & mask); }
#define __mips_asm_sdr(rt, offset, base)   { u64 mask = ~((1 << (((RecompilationContext_GetRegister(base).U64[0] + offset) % 8) * 8)) - 1); u64 value = *(long*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset); *(long*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset) = (value & ~mask) | (RecompilationContext_GetRegister(rt).U64[0] & mask); }
#define __mips_asm_swr(rt, offset, base)   *(s32*)FindRam(offset + RecompilationContext_GetRegister(base).U32[0]) = RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_cache(...)
    
// 110
#define __mips_asm_lwc1(ft, offset, base)  ctx->FPURegisters[ft] = *(float*)FindRam(offset + RecompilationContext_GetRegister(base).U32[0]);
#define __mips_asm_pref(...)
#define __mips_asm_lqc2(...)
#define __mips_asm_ld(rt, offset, base)    RecompilationContext_GetRegister(rt).S64[0] = *(long*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset);

// 111
#define __mips_asm_swc1(ft, offset, base)  *(float*)FindRam(RecompilationContext_GetRegister(base).U32[0] + offset) = ctx->FPURegisters[ft];
#define __mips_asm_sqc2(...)
#define __mips_asm_sd(rt, offset, base)    *(s64*)  FindRam(RecompilationContext_GetRegister(base).U64[0] + offset) = RecompilationContext_GetRegister(rt).S32[0];


// ===================================
// Special EE Instructions
// ===================================
// 000
#define __mips_asm_sll(rd, rt, sa)     RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0] << sa;
#define __mips_asm_srl(rd, rt, rs)     RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0] >> rs;
#define __mips_asm_sra(rd, rt, sa)     RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0] >> sa;
#define __mips_asm_sllv(rd, rt, rs)    RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0] << RecompilationContext_GetRegister(rs).U32[0];
#define __mips_asm_srlv(rd, rt, rs)    RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0] >> RecompilationContext_GetRegister(rs).U32[0];
#define __mips_asm_srav(rd, rt, rs)    RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0] >> RecompilationContext_GetRegister(rs).U32[0];

// 001
// JR (defined as a function)
// JALR (defined as a function)
#define __mips_asm_movz(rd, rs, rt) if (RecompilationContext_GetRegister(rt).U32[0] == 0) RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0];
#define __mips_asm_movn(rd, rs, rt) if (RecompilationContext_GetRegister(rt).U32[0] != 0) RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0];
void EESyscall(u8 index);
#define __mips_asm_break(...) perror("Break Instruction"); abort();
#define __mips_asm_sync

// 010
#define __mips_asm_mfhi(rd)                ctx->ee_registers[rd].U128 = ctx->hi.U128;
#define __mips_asm_mthi(rs)                ctx->hi.U128 = ctx->ee_registers[rs].U128;
#define __mips_asm_mflo(rd)                ctx->ee_registers[rd].U128 = ctx->lo.U128;
#define __mips_asm_mtlo(rs)                ctx->lo.U128 = ctx->ee_registers[rs].U128;
#define __mips_asm_dsllv(rd, rt, rs)       RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] << RecompilationContext_GetRegister(rs).U32[0];
#define __mips_asm_dsrlv(rd, rt, rs)       RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] >> RecompilationContext_GetRegister(rs).U32[0];
#define __mips_asm_dsrav(rd, rt, rs)       RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] >> RecompilationContext_GetRegister(rs).U32[0];

#define __mips_asm_mult_short(rs, rt)      { u64 result = RecompilationContext_GetRegister(rs).S32[0] * RecompilationContext_GetRegister(rt).S32[0]; ctx->hi.S32[0] = result >> 32; ctx->lo.S32[0] = result & 0xffffffffu; }
#define __mips_asm_multu_short(rs, rt)           { u64 result = RecompilationContext_GetRegister(rs).U32[0] * RecompilationContext_GetRegister(rt).U32[0]; ctx->hi.U32[0] = result >> 32; ctx->lo.U32[0] = result & 0xffffffffu; }
#define __mips_asm_div(zero, rs, rt)         ctx->lo.S32[0] = RecompilationContext_GetRegister(rs).S32[0] / RecompilationContext_GetRegister(rt).S32[0]; ctx->hi.S32[0] = RecompilationContext_GetRegister(rs).S32[0] % RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_divu(zero, rs, rt)        ctx->lo.S32[0] = RecompilationContext_GetRegister(rs).S32[0] / RecompilationContext_GetRegister(rt).S32[0]; ctx->hi.S32[0] = RecompilationContext_GetRegister(rs).S32[0] % RecompilationContext_GetRegister(rt).S32[0];

// 100
#define __mips_asm_add(rd, rs, rt)         RecompilationContext_GetRegister(rd).S32[0] = (RecompilationContext_GetRegister(rs).S32[0] + RecompilationContext_GetRegister(rt).S32[0]);
#define __mips_asm_addu(rd, rs, rt)        RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0] + RecompilationContext_GetRegister(rt).U32[0];
#define __mips_asm_sub(rd, rs, rt)         RecompilationContext_GetRegister(rd).S32[0] = RecompilationContext_GetRegister(rs).S32[0] - RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_subu(rd, rs, rt)        RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0] - RecompilationContext_GetRegister(rt).U32[0];
#define __mips_asm_and(rd, rs, rt)         RecompilationContext_GetRegister(rd).S32[0] = RecompilationContext_GetRegister(rs).S32[0] & RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_or(rd, rs, rt)          RecompilationContext_GetRegister(rd).S32[0] = RecompilationContext_GetRegister(rs).S32[0] | RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_xor(rd, rs, rt)         RecompilationContext_GetRegister(rd).S32[0] = RecompilationContext_GetRegister(rs).S32[0] ^ RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_nor(rd, rs, rt)         RecompilationContext_GetRegister(rd).U32[0] = ~(RecompilationContext_GetRegister(rs).U32[0] | RecompilationContext_GetRegister(rt).U32[0]);

// 101
#define __mips_asm_mfsa(rd)                RecompilationContext_GetRegister(rd).U32[0] = ctx->sa.U32[0];
#define __mips_asm_mtsa(rs)                ctx->sa.U32[0] = RecompilationContext_GetRegister(rs).U32[0];
#define __mips_asm_slt(rd, rs, rt)         RecompilationContext_GetRegister(rd).S32[0] = RecompilationContext_GetRegister(rs).S32[0] < RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_sltu(rd, rs, rt)        RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rs).U32[0] < RecompilationContext_GetRegister(rt).U32[0];
#define __mips_asm_dadd(rd, rs, rt)          RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rs).S64[0] + RecompilationContext_GetRegister(rt).S64[0];
#define __mips_asm_daddu(rd, rs, rt)         RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rs).S64[0] + RecompilationContext_GetRegister(rt).S64[0];
#define __mips_asm_dsub(rd, rs, rt)        RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rs).S64[0] - RecompilationContext_GetRegister(rt).S64[0];
#define __mips_asm_dsubu(rd, rs, rt)       RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rs).S64[0] - RecompilationContext_GetRegister(rt).S64[0];

// 110
#define __mips_asm_tge(rs, rt)             assert(RecompilationContext_GetRegister(rs).S32[0] <= RecompilationContext_GetRegister(rt).S32[0]);
#define __mips_asm_tgeu(rs, rt)            assert(RecompilationContext_GetRegister(rs).U32[0] <= imm);
#define __mips_asm_tlt(rs, rt)             assert(RecompilationContext_GetRegister(rs).S32[0] >= RecompilationContext_GetRegister(rt).S32[0]);
#define __mips_asm_tltu(rs, imm)           assert(RecompilationContext_GetRegister(rs).U32[0] >= imm);
#define __mips_asm_teq(rs, rt)             assert(RecompilationContext_GetRegister(rs).S32[0] != RecompilationContext_GetRegister(rt).S32[0]);
#define __mips_asm_tne(rs, rt)             assert(RecompilationContext_GetRegister(rs).S32[0] == RecompilationContext_GetRegister(rt).S32[0]);

// 111
#define __mips_asm_dsll(rd, rt, sa)        RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] << sa;
#define __mips_asm_dsrl(rd, rt, sa)        RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] >> sa;
#define __mips_asm_dsra(rd, rt, sa)        RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] >> sa;
#define __mips_asm_dsll32(rd, rt, sa)      RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] << (32 + sa);
#define __mips_asm_dsrl32(rd, rt, sa)      RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] >> (sa + 32);
#define __mips_asm_dsra32(rd, rt, sa)      RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S64[0] >> (sa + 32);

// ===================================
// REGIMM
// ===================================
// 00
#define __mips_asm_bltz(rs, branch, nextInstruction, i)     if (RecompilationContext_GetRegister(rs).S32[0] < 0) { nextInstruction; goto branch; } branch_##i: nextInstruction;
#define __mips_asm_bgez(rs, branch, nextInstruction, i)     if (RecompilationContext_GetRegister(rs).S32[0] >= 0) { nextInstruction; goto branch; } branch_##i: nextInstruction;
#define __mips_asm_bltzl(rs, branch, nextInstruction, i)    __mips_asm_bltz(rs, branch, nextInstruction, i)
#define __mips_asm_bgezl(rs, branch, nextInstruction, i)    __mips_asm_bgez(rs, branch, nextInstruction, i); // Just letting the compiler decide if its likely or not

// 01
#define __mips_asm_tgei(rs, imm)   assert(RecompilationContext_GetRegister(rs).S32[0] <= imm);
#define __mips_asm_tgeiu(rs, imm)  assert(RecompilationContext_GetRegister(rs).U32[0] <= imm);
#define __mips_asm_tlti(rs, imm)   assert(RecompilationContext_GetRegister(rs).S32[0] >= imm);
#define __mips_asm_tltiu(rs, imm)  assert(RecompilationContext_GetRegister(rs).U32[0] >= imm);
#define __mips_asm_teqi(rs, imm)   assert(RecompilationContext_GetRegister(rs).U32[0] != imm);
#define __mips_asm_tnei(rs, imm)   assert(RecompilationContext_GetRegister(rs).U32[0] == imm);

// 10
#define __mips_asm_bltzal(rs, branch, nextInstruction, i)   if (RecompilationContext_GetRegister(rs).S32[0] < 0)  { nextInstruction; perror("Unable to link"); goto branch; } branch_##i: nextInstruction;
#define __mips_asm_bgezal(rs, branch, nextInstruction, i)   if (RecompilationContext_GetRegister(rs).S32[0] >= 0) { nextInstruction; perror("Unable to link"); goto branch; } branch_##i: nextInstruction; 
#define __mips_asm_bltzall(rs, branch, nextInstruction, i)  __mips_asm_blzal(rs, branch, nextInstruction, i)
#define __mips_asm_bgezall(rs, branch, nextInstruction, i)  if ((int)a >= 0) { nextInstruction; perror("Unable to link"); goto branch; } branch_##i: nextInstruction; 

// 11
#define __mips_asm_mtsab(rs, imm) ctx->sa.U32[0] = (RecompilationContext_GetRegister(rs).U32[0] ^ imm) * 0x8;
#define __mips_asm_mtsah(rs, imm) ctx->sa.U32[0] = ((RecompilationContext_GetRegister(rs).U16[0] ^ imm) * 0x10);

// ===================================
// MMI
// https://github.com/PCSX2/pcsx2/blob/master/pcsx2/MMI.cpp
// ===================================
// 000
#define __mips_asm_madd(rd, rs, rt)            { s64 result = RecompilationContext_GetRegister(rs).S64[0] * RecompilationContext_GetRegister(rt).S64[0]; RecompilationContext_GetRegister(rd).S64[0] = result; ctx->hi.S32[0] += result >> 32; ctx->lo.S32[0] += result & 0xFFFFFFFF; }
#define __mips_asm_maddu(rd, rs, rt)           { u64 result = RecompilationContext_GetRegister(rs).U32[0] * RecompilationContext_GetRegister(rt).U32[0]; RecompilationContext_GetRegister(rd).U64[0] = result; ctx->hi.S32[0] += result >> 32; ctx->lo.U32[0] += result & 0xFFFFFFFF; }
#define __mips_asm_plzcw(rd, rs)               { RecompilationContext_GetRegister(rd).U32[0] = CountLeadingSignBits(RecompilationContext_GetRegister(rs).S32[0]) - 1; RecompilationContext_GetRegister(rd).U32[1] = CountLeadingSignBits9RecompilationContext_GetRegister(rs).S32[1]); }

// 010
#define __mips_asm_mfhi1(rd)                   ctx->ee_registers[rd].U128 = ctx->hi1.U128;
#define __mips_asm_mthi1(rs)                   ctx->hi1.U128 = ctx->ee_registers[rs].U128;
#define __mips_asm_mflo1(rd)                   ctx->ee_registers[rd].U128 = ctx->lo1.U128;
#define __mips_asm_mtlo1(rs)                   ctx->lo1.U128 = ctx->ee_registers[rs].U128;

// 011
#define __mips_asm_mult1(rd, rs, rt)           { s64 result = RecompilationContext_GetRegister(rs).S32[0] * RecompilationContext_GetRegister(rt).S32[0]; ctx->hi.S32[0] = result >> 32; ctx->lo.S32[0] = result & 0xffffffffu; }// RecompilationContext_GetRegister(rd).S32[0] = ctx->lo.S32[0]; }
#define __mips_asm_multu1(rd, rs, rt)          { u64 result = RecompilationContext_GetRegister(rs).U32[0] * RecompilationContext_GetRegister(rt).U32[0]; ctx->hi.U32[0] = result >> 32; ctx->lo.U32[0] = result & 0xffffffffu; }// RecompilationContext_GetRegister(rd).U32[0] = ctx->lo.U32[0]; }
#define __mips_asm_mult_return(rd, rs, rt)     { s64 result = RecompilationContext_GetRegister(rs).S32[0] * RecompilationContext_GetRegister(rt).S32[0]; ctx->hi.S32[0] = result >> 32; ctx->lo.S32[0] = result & 0xffffffffu; }// RecompilationContext_GetRegister(rd).S32[0] = ctx->lo.S32[0]; }
#define __mips_asm_multu_return(rd, rs, rt)    { u64 result = RecompilationContext_GetRegister(rs).U32[0] * RecompilationContext_GetRegister(rt).U32[0]; ctx->hi.U32[0] = result >> 32; ctx->lo.U32[0] = result & 0xffffffffu; }//RecompilationContext_GetRegister(rd).U32[0] = ctx->lo.U32[0]; }
#define __mips_asm_div1(zero, rs, rt)                ctx->lo1.S32[0] = RecompilationContext_GetRegister(rs).S32[0] / RecompilationContext_GetRegister(rt).S32[0]; ctx->hi1.S32[0] = RecompilationContext_GetRegister(rs).S32[0] % RecompilationContext_GetRegister(rt).S32[0];
#define __mips_asm_divu1(zero, rs, rt)               ctx->lo1.U32[0] = RecompilationContext_GetRegister(rs).U32[0] / RecompilationContext_GetRegister(rt).U32[0]; ctx->hi1.U32[0] = RecompilationContext_GetRegister(rs).U32[0] % RecompilationContext_GetRegister(rt).U32[0];

// 100
#define __mips_asm_madd1(rd, rs, rt)   { s64 result = RecompilationContext_GetRegister(rs).S32[0] * RecompilationContext_GetRegister(rt).S32[0]; RecompilationContext_GetRegister(rd).S64[0] = result; ctx->hi.S32[0] += result >> 32; ctx->lo.S32[0] += result & 0xFFFFFFFF; }
#define __mips_asm_maddu1(rd, rs, rt)  { u64 result = RecompilationContext_GetRegister(rs).U32[0] * RecompilationContext_GetRegister(rt).U32[0]; RecompilationContext_GetRegister(rd).U64[0] = result; ctx->hi.U32[0] += result >> 32; ctx->lo.U32[0] += result & 0xFFFFFFFF; }

// 110
		// case 0x02: // SLW
		// 	{
		// 		s64 TempS64 = ((u64)cpuRegs.HI.UL[0] << 32) | (u64)cpuRegs.LO.UL[0];
		//
		// 		if (TempS64 >= 0x000000007fffffffLL) {
		// 			cpuRegs.GPR.r[_Rd_].UD[0] = 0x000000007fffffffLL;
		// 		} else if (TempS64 <= -0x80000000LL) {
		// 			cpuRegs.GPR.r[_Rd_].UD[0] = 0xffffffff80000000LL;
		// 		} else {
		// 			cpuRegs.GPR.r[_Rd_].UD[0] = (s64)cpuRegs.LO.SL[0];
		// 		}
		//
		// 		TempS64 = ((u64)cpuRegs.HI.UL[2] << 32) | (u64)cpuRegs.LO.UL[2];
		//
		// 		if (TempS64 >= 0x000000007fffffffLL) {
		// 			cpuRegs.GPR.r[_Rd_].UD[1] = 0x000000007fffffffLL;
		// 		} else if (TempS64 <= -0x80000000LL) {
		// 			cpuRegs.GPR.r[_Rd_].UD[1] = 0xffffffff80000000LL;
		// 		} else {
		// 			cpuRegs.GPR.r[_Rd_].UD[1] = (s64)cpuRegs.LO.SL[2];
		// 		}
		// 	}
// Thanks PCSX2 Team :)
#define __mips_asm_PMFHLSH_CLAMP(dst, src) \
    if (src > 0x7fff) dst = 0x7fff; \
    else if (src < 0x8000) dst = 0x8000; \
    else dst = (u16)src;
#define __mips_asm_pmfhllh(rd) \
    RecompilationContext_GetRegister(rd).U16[0] = ctx->lo.U16[0]; \
    RecompilationContext_GetRegister(rd).U16[1] = ctx->lo.U16[2]; \
    RecompilationContext_GetRegister(rd).U16[2] = ctx->hi.U16[0]; \
    RecompilationContext_GetRegister(rd).U16[3] = ctx->hi.U16[2]; \
    RecompilationContext_GetRegister(rd).U16[4] = ctx->lo.U16[4]; \
    RecompilationContext_GetRegister(rd).U16[5] = ctx->lo.U16[6]; \
    RecompilationContext_GetRegister(rd).U16[6] = ctx->hi.U16[4]; \
    RecompilationContext_GetRegister(rd).U16[7] = ctx->hi.U16[6];
#define __mips_asm_pmfhluw(rd) \
    RecompilationContext_GetRegister(rd).U32[0] = ctx->lo.U32[1]; \
    RecompilationContext_GetRegister(rd).U32[1] = ctx->hi.U32[1]; \
    RecompilationContext_GetRegister(rd).U32[2] = ctx->lo.U32[3]; \
    RecompilationContext_GetRegister(rd).U32[3] = ctx->hi.U32[3];
#define __mips_asm_pmfhllw(rd) \
    RecompilationContext_GetRegister(rd).U32[0] = ctx->lo.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[1] = ctx->hi.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[2] = ctx->lo.U32[2]; \
    RecompilationContext_GetRegister(rd).U32[3] = ctx->hi.U32[2];
#define __mips_asm_pmfhlsh(rd) \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[0], ctx->lo.U32[0]); \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[1], ctx->lo.U32[1]); \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[2], ctx->hi.U32[0]); \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[3], ctx->hi.U32[1]); \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[4], ctx->lo.U32[2]); \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[5], ctx->lo.U32[3]); \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[6], ctx->hi.U32[2]); \
    PMFHLSH_CLAMP(RecompilationContext_GetRegister(rd).U16[7], ctx->hi.U32[3]);

#define __mips_asm_pmthl(rs) ctx->lo.U32[0] = RecompilationContext_GetRegister(rs).U32[0]; ctx->hi.U32[0] = RecompilationContext_GetRegister(rs).U32[1]; ctx->lo.U32[1] = RecompilationContext_GetRegister(rs).U32[2]; ctx->hi.U32[1] = RecompilationContext_GetRegister(rs).U32[3];
#define __mips_asm__psllh(rd, rt, sa, n) RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rt).U16[n] << sa;
#define __mips_asm_psllh(rd, rt, sa) __mips_asm__psllh(rd, rt, sa, 0); __mips_asm__psllh(rd, rt, sa, 1); __mips_asm__psllh(rd, rt, sa, 2); __mips_asm__psllh(rd, rt, sa, 3); __mips_asm__psllh(rd, rt, sa, 4); __mips_asm__psllh(rd, rt, sa, 5); __mips_asm__psllh(rd, rt, sa, 6); __mips_asm__psllh(rd, rt, sa, 7); 
#define __mips_asm__prslh(rd, rt, sa) RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rt).U16[n] >> sa & 0xf;
#define __mips_asm_psrlh(rd, rt, sa) __mips_asm__psrlh(rd, rt, sa, 0); __mips_asm__psrlh(rd, rt, sa, 1); __mips_asm__psrlh(rd, rt, sa, 2); __mips_asm__psrlh(rd, rt, sa, 3); __mips_asm__psrlh(rd, rt, sa, 4); __mips_asm__psrlh(rd, rt, sa, 5); __mips_asm__psrlh(rd, rt, sa, 6); __mips_asm__psrlh(rd, rt, sa, 7); 
#define __mips_asm__psrah(rd, rt, sa) RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rt).S16[n] >> sa & 0xf;
#define __mips_asm_psrah(rd, rt, sa) __mips_asm__psrah(rd, rt, sa, 0); __mips_asm__psrah(rd, rt, sa, 1); __mips_asm__psrah(rd, rt, sa, 2); __mips_asm__psrah(rd, rt, sa, 3); __mips_asm__psrah(rd, rt, sa, 4); __mips_asm__psrah(rd, rt, sa, 5); __mips_asm__psrah(rd, rt, sa, 6); __mips_asm__psrah(rd, rt, sa, 7); 
#define __mips_asm_psravw(rd, rt, rs) RecompilationContext_GetRegister(rd).S64[0] = RecompilationContext_GetRegister(rt).S32[0] >> RecompilationContext_GetRegister(rs).U32[0]; RecompilationContext_GetRegister(rd).S64[1] = RecompilationContext_GetRegister(rt).S32[1] >> RecompilationContext_GetRegister(rs).U32[1];

// 111
#define __mips_asm__psllw(rd, rt, sa, n) RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rt).U32[n] << sa;
#define __mips_asm_psllw(rd, rt, sa) __mips_asm__psllw(rd, rt, sa, 0); __mips_asm__psllw(rd, rt, sa, 1); __mips_asm__psllw(rd, rt, sa, 2); __mips_asm__psllw(rd, rt, sa, 3); 
#define __mips_asm__psrlw(rd, rt, sa, n) RecompilationContext_GetRegister(rd).U32[n] = RecompilationContext_GetRegister(rt).U32[n] >> sa;
#define __mips_asm_psrlw(rd, rt, sa) __mips_asm__psrlw(rd, rt, sa, 0); __mips_asm__psrlw(rd, rt, sa, 1); __mips_asm__psrlw(rd, rt, sa, 2); __mips_asm__psrlw(rd, rt, sa, 3); 
#define __mips_asm__psraw(rd, rt, sa, n) RecompilationContext_GetRegister(rd).U32[n] = RecompilationContext_GetRegister(rt).S32[n] >> sa;
#define __mips_asm_psraw(rd, rt, sa) __mips_asm__psraw(rd, rt, sa, 0); __mips_asm__psraw(rd, rt, sa, 1); __mips_asm__psraw(rd, rt, sa, 2); __mips_asm__psraw(rd, rt, sa, 3); 

// ===================================
// MMI0
// ===================================
// 000
#define __mips_asm__paddw(rd, rt, rs, n)    RecompilationContext_GetRegister(rd).U32[n] = RecompilationContext_GetRegister(rs).U32[n] + RecompilationContext_GetRegister(rt).U32[n];
#define __mips_asm_paddw(rd, rt, rs)       __mips_asm__paddw(rd, rt, rs, 0); __mips_asm__paddw(rd, rt, rs, 1); __mips_asm__paddw(rd, rt, rs, 2); __mips_asm__paddw(rd, rt, rs, 3); 
#define __mips_asm__psubw(rd, rt, rs, n)    RecompilationContext_GetRegister(rs).U32[n] = RecompilationContext_GetRegister(rs).U32[n] - RecompilationContext_GetRegister(rt).U32[n];
#define __mips_asm_psubw(rd, rt, rs)       __mips_asm__psubw(rd, rt, rs, 0); __mips_asm__psubw(rd, rt, rs, 1); __mips_asm__psubw(rd, rt, rs, 2); __mips_asm__psubw(rd, rt, rs, 3); 
#define __mips_asm__pcgtw(rd, rt, rs, n)    if (RecompilationContext_GetRegister(rt).S32[n] > RecompilationContext_GetRegister(rt).S32[n]) RecompilationContext_GetRegister(rd).U32[n] = 0xFFFFFFFF; else RecompilationContext_GetRegister(rd).U32[n] = 0;
#define __mips_asm_pcgtw(rd, rt, rs)       __mips_asm__pcgtw(rd, rt, rs, 0); __mips_asm__pcgtw(rd, rt, rs, 1); __mips_asm__pcgtw(rd, rt, rs, 2); __mips_asm__pcgtw(rd, rt, rs, 3); 
#define __mips_asm__pmaxw(rd, rt, rs, n)    if (RecompilationContext_GetRegister(rs).S32[n] > RecompilationContext_GetRegister(rt).S32[n]) RecompilationContext_GetRegister(rd).U32[n] = RecompilationContext_GetRegister(rs).U32[n]; else RecompilationContext_GetRegister(rd).U32[n] = RecompilationContext_GetRegister(rt).U32[n];
#define __mips_asm_pmaxw(rd, rt, rs)       __mips_asm__pmaxw(rd, rt, rs, 0); __mips_asm__pmaxw(rd, rt, rs, 1); __mips_asm__pmaxw(rd, rt, rs, 2); __mips_asm__pmaxw(rd, rt, rs, 3); 

// 001
#define __mips_asm__paddh(rd, rt, rs, n)    RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rs).U16[n] + RecompilationContext_GetRegister(rt).U16[n];
#define __mips_asm_paddh(rd, rt, rs)       __mips_asm__paddh(rd, rt, rs, 0); __mips_asm__paddh(rd, rt, rs, 1); __mips_asm__paddh(rd, rt, rs, 2); __mips_asm__paddh(rd, rt, rs, 3); __mips_asm__paddh(rd, rt, rs, 4); __mips_asm__paddh(rd, rt, rs, 5); __mips_asm__paddh(rd, rt, rs, 6); __mips_asm__paddh(rd, rt, rs, 7); 
#define __mips_asm__psubh(rd, rt, rs, n)    RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rs).U16[n] - RecompilationContext_GetRegister(rt).U16[n];
#define __mips_asm_psubh(rd, rt, rs)       __mips_asm__psubh(rd, rt, rs, 0); __mips_asm__psubh(rd, rt, rs, 1); __mips_asm__psubh(rd, rt, rs, 2); __mips_asm__psubh(rd, rt, rs, 3); __mips_asm__psubh(rd, rt, rs, 4); __mips_asm__psubh(rd, rt, rs, 5); __mips_asm__psubh(rd, rt, rs, 6); __mips_asm__psubh(rd, rt, rs, 7); 
#define __mips_asm__pcgth(rd, rt, rs, n)    if (RecompilationContext_GetRegister(rs).S16[n] > RecompilationContext_GetRegister(rt).S16[n]) RecompilationContext_GetRegister(rd).U16[n] = 0xFFFF; else RecompilationContext_GetRegister(rd).U16[n] = 0;
#define __mips_asm_pcgth(rd, rt, rs)       __mips_asm__pchth(rd, rt, rs, 0); __mips_asm__pchth(rd, rt, rs, 1); __mips_asm__pchth(rd, rt, rs, 2); __mips_asm__pchth(rd, rt, rs, 3); __mips_asm__pchth(rd, rt, rs, 4); __mips_asm__pchth(rd, rt, rs, 5); __mips_asm__pchth(rd, rt, rs, 6); __mips_asm__pchth(rd, rt, rs, 7); 
#define __mips_asm__pmaxh(rd, rt, rs, n)    if (RecompilationContext_GetRegister(rs).S16[n] > RecompilationContext_GetRegister(rt).S16[n]) RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rs).U16[n]; else RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rt).U16[n];
#define __mips_asm_pmaxh(rd, rt, rs)       __mips_asm__PMAXH(rd, rt, rs, 0); __mips_asm__PMAXH(rd, rt, rs, 1); __mips_asm__PMAXH(rd, rt, rs, 2); __mips_asm__PMAXH(rd, rt, rs, 3); __mips_asm__PMAXH(rd, rt, rs, 4); __mips_asm__PMAXH(rd, rt, rs, 5); __mips_asm__PMAXH(rd, rt, rs, 6); __mips_asm__PMAXH(rd, rt, rs, 7); 

// 010
#define __mips_asm__paddb(rd, rt, rs, n)   RecompilationContext_GetRegister(rd).S8[n] = RecompilationContext_GetRegister(rs).S8[n] + RecompilationContext_GetRegister(rt).S8[n];
#define __mips_asm_paddb(rd, rt, rs)       { for (int i = 0; i < 16; i ++) __mips_asm__paddb(rd, rt, rs, i); }
#define __mips_asm__psubb(rd, rt, rs)      RecompilationContext_GetRegister(rd).S8[n] = RecompilationContext_GetRegister(rs).S8[n] - RecompilationContext_GetRegister(rt).S8[n];
#define __mips_asm_psubb(rd, rt, rs)       { for (int _i = 0; i < 16; i ++) __mips_asm__psubb(rd, rt, rs, i); }
#define __mips_asm__pcgtb(rd, rt, rs, n)   if (RecompilationContext_GetRegister(rs).S8[n] > RecompilationContext_GetRegister(rt).S8[n]) RecompilationContext_GetRegister(rd).U8[n] = 0xff; else RecompilationContext_GetRegister(rd).U8[n] = 0;
#define __mips_asm_pcgtb(rd, rt, rs)       { for (int i = 0; i < 16; i ++) __mips_asm__pcgtb(rd, rt, rs, i); }

// 100
#define __mips_asm__paddsw(rd, rt, rs, n) { s64 tmp = (s64)RecompilationContext_GetRegister(rs).S32[n] + (s64)RecompilationContext_GetRegister(rt).S32[n]; if (tmp > 0x7FFFFFFF) RecompilationContext_GetRegister(rd).U32[n] = 0x7FFFFFFF; else if (tmp < (s32)0x80000000) RecompilationContext_GetRegister(rd).U32[n] = 0x80000000LL; else RecompilationContext_GetRegister(rd).U32[n] = (s32)tmp; }
#define __mips_asm_paddsw(rd, rt, rs) __mips_asm__paddsw(rd, rt, rs, 0); __mips_asm__paddsw(rd, rt, rs, 1); __mips_asm__paddsw(rd, rt, rs, 2); __mips_asm__paddsw(rd, rt, rs, 3); 
#define __mips_asm__psubsw(rd, rt, rs, n) { s64 tmp = (s64)RecompilationContext_GetRegister(rs).S32[n] - (s64)RecompilationContext_GetRegister(rt).S32[n]; if (tmp >= 0x7FFFFFFF) RecompilationContext_GetRegister(rd).U32[n] = 0x7FFFFFFF; else if (tmp < (s32)0x80000000) RecompilationContext_GetRegister(rd).U32[n] = 0x80000000; else RecompilationContext_GetRegister(rd).U32[n] = (s32)tmp; }
#define __mips_asm_psubsw(rd, rt, rs) __mips_asm__psubsw(rd, rt, rs, 0); __mips_asm__psubsw(rd, rt, rs, 1); __mips_asm__psubsw(rd, rt, rs, 2); __mips_asm__psubsw(rd, rt, rs, 3); 
#define __mips_asm_pextlw(rd, rt, rs) RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0]; RecompilationContext_GetRegister(rd).U32[1] = RecompilationContext_GetRegister(rs).U32[0]; RecompilationContext_GetRegister(rd).U32[2] = RecompilationContext_GetRegister(rt).U32[1]; RecompilationContext_GetRegister(rd).U32[3] = RecompilationContext_GetRegister(rs).U32[1];
#define __mips_asm_ppacw(rd, rt, rs) RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[0]; RecompilationContext_GetRegister(rd).U32[1] = RecompilationContext_GetRegister(rt).U32[2]; RecompilationContext_GetRegister(rd).U32[2] = RecompilationContext_GetRegister(rs).U32[0]; RecompilationContext_GetRegister(rd).U32[3] =RecompilationContext_GetRegister(rs).U32[2];

// 101
#define __mips_asm__paddsh(rd, rt, rs, n) { s32 tmp = (s32)RecompilationContext_GetRegister(rs).S16[n] + (s32)RecompilationContext_GetRegister(rt).S16[n]; if (tmp > 0x7FFF) RecompilationContext_GetRegister(rd).U16[n] = 0x7FFF; else if (tmp < (s32)0xffff800) RecompilationContext_GetRegister(rd).U16[n] = 0x8000; else RecompilationContext_GetRegister(rd).U16[n] = (s16)tmp; }
#define __mips_asm_paddsh(rd, rt, rs) __mips_asm__paddsh(rd, rt, rs, 0); __mips_asm__paddsh(rd, rt, rs, 1); __mips_asm__paddsh(rd, rt, rs, 2); __mips_asm__paddsh(rd, rt, rs, 3); __mips_asm__paddsh(rd, rt, rs, 4); __mips_asm__paddsh(rd, rt, rs, 5); __mips_asm__paddsh(rd, rt, rs, 6); __mips_asm__paddsh(rd, rt, rs, 7); 
#define __mips_asm__psubsh(rd, rt, rs, n) { s32 tmp = (s32)RecompilationContext_GetRegister(rs).S16[n] - RecompilationContext_GetRegister(rt).S16[n]; if (tmp >= 0x7fff) RecompilationContext_GetRegister(rd).U16[n] = 0x7FFF; else if (tmp < (s32)0xFFFF8000) RecompilationContext_GetRegister(rd).U16[n] = 0x8000; else RecompilationContext_GetRegister(rd).U16[n] = (s16)tmp; }
#define __mips_asm_psubsh(rd, rt, rs) __mips_asm__psubsh(rd, rt, rs, 0); __mips_asm__psubsh(rd, rt, rs, 1); __mips_asm__psubsh(rd, rt, rs, 2); __mips_asm__psubsh(rd, rt, rs, 3); __mips_asm__psubsh(rd, rt, rs, 4); __mips_asm__psubsh(rd, rt, rs, 5); __mips_asm__psubsh(rd, rt, rs, 6); __mips_asm__psubsh(rd, rt, rs, 7); 
#define __mips_asm_pextlh(rd, rt, rs) RecompilationContext_GetRegister(rd).U16[0] = RecompilationContext_GetRegister(rt).U16[0]; RecompilationContext_GetRegister(rd).U16[1] = RecompilationContext_GetRegister(rs).U16[0]; RecompilationContext_GetRegister(rd).U16[2] = RecompilationContext_GetRegister(rt).U16[1]; RecompilationContext_GetRegister(rd).U16[3] = RecompilationContext_GetRegister(rs).U16[1]; RecompilationContext_GetRegister(rd).U16[4] = RecompilationContext_GetRegister(rt).U16[2]; RecompilationContext_GetRegister(rd).U16[5] = RecompilationContext_GetRegister(rs).U16[2]; RecompilationContext_GetRegister(rd).U16[6] = RecompilationContext_GetRegister(rt).U16[3]; RecompilationContext_GetRegister(rd).U16[7] = RecompilationContext_GetRegister(rs).U16[3];
#define __mips_asm_ppach(rd, rt, rs)  RecompilationContext_GetRegister(rd).U16[0] = RecompilationContext_GetRegister(rt).U16[0]; RecompilationContext_GetRegister(rd).U16[1] = RecompilationContext_GetRegister(rt).U16[2]; RecompilationContext_GetRegister(rd).U16[2] = RecompilationContext_GetRegister(rt).U16[4]; RecompilationContext_GetRegister(rd).U16[3] = RecompilationContext_GetRegister(rt).U16[6]; RecompilationContext_GetRegister(rd).U16[4] = RecompilationContext_GetRegister(rs).U16[0]; RecompilationContext_GetRegister(rd).U16[5] = RecompilationContext_GetRegister(rs).U16[2]; RecompilationContext_GetRegister(rd).U16[6] = RecompilationContext_GetRegister(rs).U16[4]; RecompilationContext_GetRegister(rd).U16[7] = RecompilationContext_GetRegister(rs).U16[6];

// 110
#define __mips_asm__paddsb(rd, rt, rs, n) { s16 tmp = (s16)RecompilationContext_GetRegister(rs).S8[n] + (s16)RecompilationContext_GetRegister(rt).S8[n]; if (tmp > 0x7F) RecompilationContext_GetRegister(rd).U8[n] = 0x7F; else if (tmp < (s16)-128) RecompilationContext_GetRegister(rd).U18[n] = 0x80; else RecompilationContext_GetRegister(rd).U8[n] = (s8)tmp; }
#define __mips_asm_paddsb(rd, rt, rs) for (int i = 0; i < 16; i++) __mips_asm__paddsb(i); 
#define __mips_asm__psubsb(rd, rt, rs, n) { s16 tmp = (s16)RecompilationContext_GetRegister(rs).S8[n] - (s16)RecompilationContext_GetRegister(rt).S8[n]; if (tmp > 0x7F) RecompilationContext_GetRegister(rd).U8[n] = 0x7F; else if (tmp < (s16)-128) RecompilationContext_GetRegister(rd).U18[n] = 0x80; else RecompilationContext_GetRegister(rd).U8[n] = (s8)tmp; }
#define __mips_asm_psubsb(rd, rt, rs) \
    for (int i = 0; i < 16; i++) \
        __mips_asm__psubsb(i); 
#define __mips_asm_pextlb(rd, rt, rs) \
    RecompilationContext_GetRegister(rd).U8[0] = RecompilationContext_GetRegister(rt).U8[0]; \
    RecompilationContext_GetRegister(rd).U8[1] = RecompilationContext_GetRegister(rs).U8[0]; \
    RecompilationContext_GetRegister(rd).U8[2] = RecompilationContext_GetRegister(rt).U8[1]; \
    RecompilationContext_GetRegister(rd).U8[3] = RecompilationContext_GetRegister(rs).U8[1]; \
    RecompilationContext_GetRegister(rd).U8[4] = RecompilationContext_GetRegister(rt).U8[2]; \
    RecompilationContext_GetRegister(rd).U8[5] = RecompilationContext_GetRegister(rs).U8[2]; \
    RecompilationContext_GetRegister(rd).U8[6] = RecompilationContext_GetRegister(rt).U8[3]; \
    RecompilationContext_GetRegister(rd).U8[7] = RecompilationContext_GetRegister(rs).U8[3]; \
    RecompilationContext_GetRegister(rd).U8[8] = RecompilationContext_GetRegister(rt).U8[4]; \
    RecompilationContext_GetRegister(rd).U8[9] = RecompilationContext_GetRegister(rs).U8[4]; \
    RecompilationContext_GetRegister(rd).U8[10] = RecompilationContext_GetRegister(rt).U8[5]; \
    RecompilationContext_GetRegister(rd).U8[11] = RecompilationContext_GetRegister(rs).U8[5]; \
    RecompilationContext_GetRegister(rd).U8[12] = RecompilationContext_GetRegister(rt).U8[6]; \
    RecompilationContext_GetRegister(rd).U8[13] = RecompilationContext_GetRegister(rs).U8[6]; \
    RecompilationContext_GetRegister(rd).U8[14] = RecompilationContext_GetRegister(rt).U8[7]; \
    RecompilationContext_GetRegister(rd).U8[15] = RecompilationContext_GetRegister(rs).U8[7];

#define __mips_asm_ppacb(rd, rt, rs) \
    RecompilationContext_GetRegister(rd).U8[0] = RecompilationContext_GetRegister(rt).U8[0]; \
    RecompilationContext_GetRegister(rd).U8[1] = RecompilationContext_GetRegister(rt).U8[2]; \
    RecompilationContext_GetRegister(rd).U8[2] = RecompilationContext_GetRegister(rt).U8[4]; \
    RecompilationContext_GetRegister(rd).U8[3] = RecompilationContext_GetRegister(rt).U8[6]; \
    RecompilationContext_GetRegister(rd).U8[4] = RecompilationContext_GetRegister(rt).U8[8]; \
    RecompilationContext_GetRegister(rd).U8[5] = RecompilationContext_GetRegister(rt).U8[10]; \
    RecompilationContext_GetRegister(rd).U8[6] = RecompilationContext_GetRegister(rt).U8[12]; \
    RecompilationContext_GetRegister(rd).U8[7] = RecompilationContext_GetRegister(rt).U8[14]; \
    RecompilationContext_GetRegister(rd).U8[8] = RecompilationContext_GetRegister(rs).U8[0]; \
    RecompilationContext_GetRegister(rd).U8[9] = RecompilationContext_GetRegister(rs).U8[2]; \
    RecompilationContext_GetRegister(rd).U8[10] = RecompilationContext_GetRegister(rs).U8[4]; \
    RecompilationContext_GetRegister(rd).U8[11] = RecompilationContext_GetRegister(rs).U8[6]; \
    RecompilationContext_GetRegister(rd).U8[12] = RecompilationContext_GetRegister(rs).U8[8]; \
    RecompilationContext_GetRegister(rd).U8[13] = RecompilationContext_GetRegister(rs).U8[10]; \
    RecompilationContext_GetRegister(rd).U8[14] = RecompilationContext_GetRegister(rs).U8[12]; \
    RecompilationContext_GetRegister(rd).U8[15] = RecompilationContext_GetRegister(rs).U8[14];

// 111
#define __mips_asm__pext5(rd, rt, n) \
    RecompilationContext_GetRegister(rd).U32[n] = ((RecompilationContext_GetRegister(rt).U32[n] & 0x1F) << 3) | \
                ((RecompilationContext_GetRegister(rt).U32[n] & 0x3E0) << 6) | \
                ((RecompilationContext_GetRegister(rt).U32[n] & 0x7c00) << 9) | \
                ((RecompilationContext_GetRegister(rt).U32[n] & 0x8000) << 16);
#define __mips_asm_pext5(rd, rt) __mips_asm__pext5(rd, rt, 0); __mips_asm__pext5(rd, rt, 1); __mips_asm__pext5(rd, rt, 2); __mips_asm__pext5(rd, rt, 3); 

#define __mips_asm__ppac5(rd, rt, n) \
    RecompilationContext_GetRegister(rd).U32[n] = ((RecompilationContext_GetRegister(rt).U32[n] & 0x1F) << 3) | \
                ((RecompilationContext_GetRegister(rt).U32[n] & 0x3E0) << 6) | \
                ((RecompilationContext_GetRegister(rt).U32[n] & 0x7c00) << 9) | \
                ((RecompilationContext_GetRegister(rt).U32[n] & 0x8000) << 16);
#define __mips_asm_ppac5(rd, rt) __mips_asm__ppac5(rd, rt, 0); __mips_asm__ppac5(rd, rt, 1); __mips_asm__ppac5(rd, rt, 2); __mips_asm__ppac5(rd, rt, 3); 

// ===================================
// MMI1
// ===================================
// 000
#define __mips_asm__pabsw(rd, rt, n) if (RecompilationContext_GetRegister(rd).U32[n] == 0x80000000) RecompilationContext_GetRegister(rd).U32[n] = 0x7fffffff; else if (RecompilationContext_GetRegister(rt).S32[n] < 0) RecompilationContext_GetRegister(rd).U32[n] = -RecompilationContext_GetRegister(rt).S32[n] else RecompilationContext_GetRegister(rd).U32[n] = RecompilationContext_GetRegister(rt).S32[n];
#define __mips_asm_pabsw(rd, rt) __mips_asm__pabsw(rd, rt, 0); __mips_asm__pabsw(rd, rt, 1); __mips_asm__pabsw(rd, rt, 2); __mips_asm__pabsw(rd, rt, 3); 
#define __mips_asm__pceqw(rd, rt, rs, n) if (RecompilationContext_GetRegister(rs).U32[n] == RecompilationContext_GetRegister(rt).U32[n]) RecompilationContext_GetRegister(rd).U32[n] = 0xFFFFFFFF; else RecompilationContext_GetRegister(rd).U32[n] = 0;
#define __mips_asm_pceqw(rd, rt, rs) __mips_asm__pceqw(rd, rt, rs, 0); __mips_asm__pceqw(rd, rt, rs, 1); __mips_asm__pceqw(rd, rt, rs, 2); __mips_asm__pceqw(rd, rt, rs, 3); 
#define __mips_asm__pminw(rd, rt, rs, n) if (RecompilationContext_GetRegister(rs).S32[n] < RecompilationContext_GetRegister(rt).S32[n]) RecompilationContext_GetRegister(rd).S32[n] = RecompilationContext_GetRegister(rs).S32[n]; else RecompilationContext_GetRegister(rd).S32[n] = RecompilationContext_GetRegister(rt).S32[n];
#define __mips_asm_pminw(rd, rt, rs) __mips_asm__pminw(rd, rt, rs, 0); __mips_asm__pminw(rd, rt, rs, 1); __mips_asm__pminw(rd, rt, rs, 2); __mips_asm__pminw(rd, rt, rs, 3); 

// 001
#define __mips_asm_padsbh(rd, rt, rs) __mips_asm__psubh(rd, rt, rs, 0); __mips_asm__psubh(rd, rt, rs, 1); __mips_asm__psubh(rd, rt, rs, 2); __mips_asm__psubh(rd, rt, rs, 3); __mips_asm__psubh(rd, rt, rs, 4); __mips_asm__psubh(rd, rt, rs, 5); __mips_asm__psubh(rd, rt, rs, 6); __mips_asm__psubh(rd, rt, rs, 7); 
#define __mips_asm__pabsh(rd, rt, n) \
    if (RecompilationContext_GetRegister(rt).U16[n] == 0x8000) \
         RecompilationContext_GetRegister(rd).U16[n] = 0x7FFF; \
    else if (RecompilationContext_GetRegister(rt).S16[n] < 0) \
         RecompilationContext_GetRegister(rd).U16[n] = -RecompilationContext_GetRegister(rt).S16[n]; \
    else RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rt).S16[n];
#define __mips_asm_pabsh(rd, rt) __mips_asm__pabsh(rd, rt, 0); __mips_asm__pabsh(rd, rt, 1); __mips_asm__pabsh(rd, rt, 2); __mips_asm__pabsh(rd, rt, 3); __mips_asm__pabsh(rd, rt, 4); __mips_asm__pabsh(rd, rt, 5); __mips_asm__pabsh(rd, rt, 6); __mips_asm__pabsh(rd, rt, 7); 
#define __mips_asm__pceqh(rd, rt, rs, n) \
    if (RecompilationContext_GetRegister(rs).U16[n] == RecompilationContext_GetRegister(rt).U16[n]; \
        RecompilationContext_GetRegister(rd).U16[n] = 0xffff; \
    else \
        RecompilationContext_GetRegister(rd).U16[n] = 0;
#define __mips_asm_pceqh(rd, rt, rs) __mips_asm__pceqh(rd, rt, rs, 0); __mips_asm__pceqh(rd, rt, rs, 1); __mips_asm__pceqh(rd, rt, rs, 2); __mips_asm__pceqh(rd, rt, rs, 3); __mips_asm__pceqh(rd, rt, rs, 4); __mips_asm__pceqh(rd, rt, rs, 5); __mips_asm__pceqh(rd, rt, rs, 6); __mips_asm__pceqh(rd, rt, rs, 7); 
#define __mips_asm__pminh(rd, rt, rs) \
        if (RecompilationContext_GetRegister(rs).S16[n] < RecompilationContext_GetRegister(rt).S16[n]) \
            RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rs).U16[n]; \
        else RecompilationContext_GetRegister(rd).U16[n] = RecompilationContext_GetRegister(rt).U16[n];
#define __mips_asm_pminh(rd, rt, rs) __mips_asm__pminh(rd, rt, rs, 0); __mips_asm__pminh(rd, rt, rs, 1); __mips_asm__pminh(rd, rt, rs, 2); __mips_asm__pminh(rd, rt, rs, 3); __mips_asm__pminh(rd, rt, rs, 4); __mips_asm__pminh(rd, rt, rs, 5); __mips_asm__pminh(rd, rt, rs, 6); __mips_asm__pminh(rd, rt, rs, 7); 

// 010
#define __mips_asm__pceqb(rd, rt, rs, n) \
        if (RecompilationContext_GetRegister(rs).U8[n] == RecompilationContext_GetRegister(rt).U8[n]) \
            RecompilationContext_GetRegister(rd).U8[n] = 0xFF; \
        else RecompilationContext_GetRegister(rd).U8[n] = 0;
#define __mips_asm_pceqb(rd, rt, rs) for (int i = 0; i < 16; i++) __mips_asm__pceqb(rd, rt, rs, i);

// 100
#define __mips_asm__padduw(rd, rs, rt, n) \
        { \
            s64 tmp = (s64)RecompilationContext_GetRegister(rs).U32[n] + (s64)RecompilationContext_GetRegister(rt).U32[n]; \
            if (tmp > 0xFFFFFFFF) \
                RecompilationContext_GetRegister(rd).U32[n] = 0xFFFFFFFF; \
            else RecompilationContext_GetRegister(rd).U32[n] = (u32)tmp; \
        }
#define __mips_asm_padduw(rd, rs, rt) __mips_asm__padduw(rd, rs, rt, 0); __mips_asm__padduw(rd, rs, rt, 1); __mips_asm__padduw(rd, rs, rt, 2); __mips_asm__padduw(rd, rs, rt, 3); 
#define __mips_asm__psubuw(rd, rs, rt, n) \
        { \
            s64 tmp = (s64)RecompilationContext_GetRegister(rs).U32[n] - (64)RecompilationContext_GetRegister(rt).U32[n]; \
            if (tmp <= 0) \
                RecompilationContext_GetRegister(rd).U32[n] = 0; \
            else RecompilationContext_GetRegister(rd).U32[n] = (u32)tmp; \
        }
#define __mips_asm_psubuw(rd, rs, rt) __mips_asm__psubuw(rd, rs, rt, 0); __mips_asm__psubuw(rd, rs, rt, 1); __mips_asm__psubuw(rd, rs, rt, 2); __mips_asm__psubuw(rd, rs, rt, 3); 
#define __mips_asm_pextuw(rd, rs, rt) \
        RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[2]; \
        RecompilationContext_GetRegister(rd).U32[1] = RecompilationContext_GetRegister(rt).U32[2]; \
        RecompilationContext_GetRegister(rd).U32[2] = RecompilationContext_GetRegister(rs).U32[3]; \
        RecompilationContext_GetRegister(rd).U32[3] = RecompilationContext_GetRegister(rs).U32[3]; 

// 101
#define __mips_asm__padduh(rd, rs, rt, n) \
        { \
            s32 tmp = (s32)RecompilationContext_GetRegister(rs).U16[n] + (s32)RecompilationContext_GetRegister(rt).U16[n]; \
            if (tmp > 0xFFFF) \
                RecompilationContext_GetRegister(rd).U16[n] = 0xFFFF; \
            else RecompilationContext_GetRegister(rd).U16[n] = (u16)tmp; \
        }
#define __mips_asm_padduh(rd, rs, rt) __mips_asm__padduh(rd, rs, rt, 0); __mips_asm__padduh(rd, rs, rt, 1); __mips_asm__padduh(rd, rs, rt, 2); __mips_asm__padduh(rd, rs, rt, 3); __mips_asm__padduh(rd, rs, rt, 4); __mips_asm__padduh(rd, rs, rt, 5); __mips_asm__padduh(rd, rs, rt, 6); __mips_asm__padduh(rd, rs, rt, 7); 
#define __mips_asm__psubuh(rd, rs, rt, n) \
        { \
            s32 tmp = (s32)RecompilationContext_GetRegister(rs).U16[n] + (s32)RecompilationContext_GetRegister(rt).U16[n]; \
            if (tmp <= 0) \
                RecompilationContext_GetRegister(rd).U16[n] = 0; \
            else RecompilationContext_GetRegister(rd).U16[n] = (u16)tmp; \
        }
#define __mips_asm_psubuh(rd, rs, rt) __mips_asm__psubuh(rd, rs, rt, 0); __mips_asm__psubuh(rd, rs, rt, 1); __mips_asm__psubuh(rd, rs, rt, 2); __mips_asm__psubuh(rd, rs, rt, 3); __mips_asm__psubuh(rd, rs, rt, 4); __mips_asm__psubuh(rd, rs, rt, 5); __mips_asm__psubuh(rd, rs, rt, 6); __mips_asm__psubuh(rd, rs, rt, 7); 
#define __mips_asm_pextuh(rd, rs, rt) \
        RecompilationContext_GetRegister(rd).U16[0] = RecompilationContext_GetRegister(rt).U16[4]; \
        RecompilationContext_GetRegister(rd).U16[1] = RecompilationContext_GetRegister(rs).U16[4]; \
        RecompilationContext_GetRegister(rd).U16[2] = RecompilationContext_GetRegister(rt).U16[5]; \
        RecompilationContext_GetRegister(rd).U16[3] = RecompilationContext_GetRegister(rs).U16[5]; \
        RecompilationContext_GetRegister(rd).U16[4] = RecompilationContext_GetRegister(rt).U16[6]; \
        RecompilationContext_GetRegister(rd).U16[5] = RecompilationContext_GetRegister(rs).U16[6]; \
        RecompilationContext_GetRegister(rd).U16[6] = RecompilationContext_GetRegister(rt).U16[7]; \
        RecompilationContext_GetRegister(rd).U16[7] = RecompilationContext_GetRegister(rs).U16[7];

// 110
#define __mips_asm__paddub(rd, rs, rt, n) \
        { \
            u16 tmp = (s16)RecompilationContext_GetRegister(rs).U8[n] + (s16)RecompilationContext_GetRegister(rt).U8[n]; \
            if (tmp > 0xFF) \
                RecompilationContext_GetRegister(rd).U8[n] = 0xFF; \
            else RecompilationContext_GetRegister(rd).U8[n] = (u8)tmp; \
        }
#define __mips_asm_paddub(rd, rs, rt) for (int i = 0; i < 16; i++) __mips_asm__paddub(rd, rs, rt, i);
#define __mips_asm__psubub(rd, rs, rt, n) \
        { \
            u16 tmp = (s16)RecompilationContext_GetRegister(rs).U8[n] - (s16)RecompilationContext_GetRegister(rt).U8[n]; \
            if (tmp <= 0) \
                RecompilationContext_GetRegister(rd).U8[n] = 0; \
            else RecompilationContext_GetRegister(rd).U8[n] = (u8)tmp; \
        }
#define __mips_asm_psubub(rd, rs, rt) for (int i = 0; i < 16; i++) __mips_asm__psubub(rd, rs, rt, i);
#define __mips_asm_pextub(rd, rs, rt) \
        RecompilationContext_GetRegister(rd).U8[0] = RecompilationContext_GetRegister(rt).U8[8]; \
        RecompilationContext_GetRegister(rd).U8[1] = RecompilationContext_GetRegister(rs).U8[8]; \
        RecompilationContext_GetRegister(rd).U8[2] = RecompilationContext_GetRegister(rt).U8[9]; \
        RecompilationContext_GetRegister(rd).U8[3] = RecompilationContext_GetRegister(rs).U8[9]; \
        RecompilationContext_GetRegister(rd).U8[4] = RecompilationContext_GetRegister(rt).U8[10]; \
        RecompilationContext_GetRegister(rd).U8[5] = RecompilationContext_GetRegister(rs).U8[10]; \
        RecompilationContext_GetRegister(rd).U8[6] = RecompilationContext_GetRegister(rt).U8[11]; \
        RecompilationContext_GetRegister(rd).U8[7] = RecompilationContext_GetRegister(rs).U8[11]; \
        RecompilationContext_GetRegister(rd).U8[8] = RecompilationContext_GetRegister(rt).U8[12]; \
        RecompilationContext_GetRegister(rd).U8[9] = RecompilationContext_GetRegister(rs).U8[12]; \
        RecompilationContext_GetRegister(rd).U8[10] = RecompilationContext_GetRegister(rt).U8[13]; \
        RecompilationContext_GetRegister(rd).U8[11] = RecompilationContext_GetRegister(rs).U8[13]; \
        RecompilationContext_GetRegister(rd).U8[12] = RecompilationContext_GetRegister(rt).U8[14]; \
        RecompilationContext_GetRegister(rd).U8[13] = RecompilationContext_GetRegister(rs).U8[14]; \
        RecompilationContext_GetRegister(rd).U8[14] = RecompilationContext_GetRegister(rt).U8[15]; \
        RecompilationContext_GetRegister(rd).U8[15] = RecompilationContext_GetRegister(rs).U8[15];

#define __mips_asm_qfsrv(rd, rs, rt) \
{  \
    u32 saAmnt = ctx->sa.U64[0] << 3; \
    if (saAmnt == 0) { \
        RecompilationContext_GetRegister(rd).U64[0] = RecompilationContext_GetRegister(rt).U64[0]; \
        RecompilationContext_GetRegister(rd).U64[1] = RecompilationContext_GetRegister(rt).U64[1]; \
    } else if (saAmnt < 64) { \
        RecompilationContext_GetRegister(rd).U64[0] >>= saAmnt; \
        RecompilationContext_GetRegister(rd).U64[1] >>= saAmnt; \
        RecompilationContext_GetRegister(rd).U64[0] |= RecompilationContext_GetRegister(rd).U64[1] << (64 - saAmnt); \
        RecompilationContext_GetRegister(rd).U64[1] |= RecompilationContext_GetRegister(rd).U64[0] << (64 - saAmnt); \
    } else if (saAmnt != 64) \
    { \
        RecompilationContext_GetRegister(rd).U64[0] |= RecompilationContext_GetRegister(rs).U64[0] << (128u - saAmnt); \
        RecompilationContext_GetRegister(rd).U64[0] |= RecompilationContext_GetRegister(rs).U64[1] << (128u - saAmnt); \
    } else { \
        RecompilationContext_GetRegister(rd).U64[0] = RecompilationContext_GetRegister(rt).U64[1] >> (saAmnt - 64); \
        RecompilationContext_GetRegister(rd).U64[1] = RecompilationContext_GetRegister(rs).U64[0] >> (saAmnt - 64); \
    } \
 \
}


// ===================================
// MMI2
// ===================================
// 000
#define __mips_asm__pmaddw(rd, rs, rt, dd, ss) \
{ \
    s64 tmp (s64)RecompilationContext_GetRegister(rs).S32[ss] * (s64)RecompilationContext_GetRegister(rt).S32[ss]; \
    s64 tmp2 = temp + ((s64)ctx->hi.S32[ss] << 32); \
 \
    if (ss == 0) \
    { \
        if ((RecompilationContext_GetRegister(rt).S32[ss] & 0x7FFFFFFF) == 0 || (RecompilationContext_GetRegister(rt).S32[ss] & 0x7FFFFFFF == 0x7FFFFFFF) && \
                RecompilationContext_GetRegister(rs).S32[ss] != RecompilationContext_GetRegister(rt).S32[ss]) \
            tmp2 += 0x70000000; \
    } \
 \
    tmp2 = (s32)(tmp2) / 0xFFFFFFFF; \
 \
    ctx->lo.S64[dd] = (s32)(tmp & 0xFFFFFFFF) + ctx->lo.S32[ss]; \
    ctx->hi.S64[dd] = (s32)tmp2; \
 \
    rd.U32[dd * 2] = ctx->lo.U32[dd * 2]; \
    rd.U32[dd * 2 + 1] = ctx->hi.U32[dd * 2]; \
}
#define __mips_asm_pmaddw(rd, rs, rt) __mips_asm__pmaddw(0, 0); __mips_asm__pmaddw(1, 2);
#define __mips_asm_psllvw(rd, rs, rt) \
    RecompilationContext_GetRegister(rd).s64[0] = (s64)(s32)(RecompilationContext_GetRegister(rt).U32[0] << (RecompilationContext_GetRegister(rs).U32[0] & 0x1F): \
    RecompilationContext_GetRegister(rd).s64[1] = (s64)(s32)(RecompilationContext_GetRegister(rt).U32[2] << (RecompilationContext_GetRegister(rs).U32[2] & 0x1F):
#define __mips_asm_psrlvw(rd, rs, rt) \
    RecompilationContext_GetRegister(rd).S64[0] = (s64)(s32)(RecompilationContext_GetRegister(rt).U32[0] >> (RecompilationContext_GetRegister(rs).U32[0] & 0x1F); \
    RecompilationContext_GetRegister(rd).S64[0] = (s64)(s32)(RecompilationContext_GetRegister(rt).U32[1] >> (RecompilationContext_GetRegister(rs).U32[2] & 2x1F);

// 001
#define __mips_asm__pmsubw(rd, rs, rt, dd, ss) \
{ \
    s64 tmp = (s64)RecompilationContext_GetRegister(rs).S32[ss] * (s64)RecompilationContext_GetRegister(rt).S32[ss]; \
    s64 tmp2 = ((s64)ctx->hi.S32[ss] << 32) - tmp; \
 \
    tmp2 = (s32)(tmp2 / 0xFFFFFFFF); \
 \
    ctx->lo.S64[dd] = ctx->lo.S32[ss] - (s32)(tmp & 0xFFFFFFFF); \
    ctx->hi.S64[dd] = (s32)tmp2; \
 \
    rd.U32[dd * 2] = ctx->lo.U32[dd * 2]; \
    rd.U32[dd * 2 + 1] = ctx->hi.U32[dd * 2]; \
}
#define __mips_asm_pmsubw(rd, rs, rt) __mips_asm__pmsubw(0,0); __mips_asm__pmsubw(1, 2);

// 010
#define __mips_asm_pmflo(rd) \
    RecompilationContext_GetRegister(rd).U64[0] = ctx->lo.U64[0]; \
    RecompilationContext_GetRegister(rd).U64[1] = ctx->lo.U64[1];
#define __mips_asm_pmfhi(rd) \
    RecompilationContext_GetRegister(rd).U64[0] = ctx->hi.U64[0]; \
    RecompilationContext_GetRegister(rd).U64[1] = ctx->hi.U64[1];
#define __mips_asm_pinth(rd, rs, rt) \
    RecompilationContext_GetRegister(rd).U16[0] = RecompilationContext_GetRegister(rt).U16[0]; \
    RecompilationContext_GetRegister(rd).U16[1] = RecompilationContext_GetRegister(rs).U16[4]; \
    RecompilationContext_GetRegister(rd).U16[2] = RecompilationContext_GetRegister(rt).U16[1]; \
    RecompilationContext_GetRegister(rd).U16[3] = RecompilationContext_GetRegister(rs).U16[5]; \
    RecompilationContext_GetRegister(rd).U16[4] = RecompilationContext_GetRegister(rt).U16[2]; \
    RecompilationContext_GetRegister(rd).U16[5] = RecompilationContext_GetRegister(rs).U16[6]; \
    RecompilationContext_GetRegister(rd).U16[6] = RecompilationContext_GetRegister(rt).U16[3]; \
    RecompilationContext_GetRegister(rd).U16[7] = RecompilationContext_GetRegister(rs).U16[7];

// 011
#define __mips_asm__pmultw(rd, rs, rt, dd, ss) \
    { \
        s64 tmp = (s64)RecompilationContext_GetRegister(rs).S32[ss] * (s64)RecompilationContext_GetRegister(rt).S32[ss]; \
        ctx->lo.U64[dd] = (s32)(tmp & 0xFFFFFFFF); \
        ctx->hi.U64[dd] = (s32)(tmp >> 32); \
        RecompilationContext_GetRegister(rd).S64[dd] = tmp; \
    }

#define __mips_asm_pmultw(rd, rs, rt) __mips_asm__pmultw(rd, rs, rt, 0, 0); __mips_asm__pmultw(rd, rs, rt, 1, 2); 
#define __mips_asm__pdivw(rd, rs, rt, dd, ss) \
    if (RecompilationContext_GetRegister(rs).U32[ss] == 0x80000000 && RecompilationContext_GetRegister(rt).U32[ss] == 0xFFFFFFFF) \
    { \
        ctx->lo.S64[dd] = (s32)0x80000000; \
        ctx->hi.S64[dd] = 0; \
    } \
    else if (RecompilationContext_GetRegister(rt).S32[ss] != 0) \
    { \
        ctx->lo.S64[dd] = RecompilationContext_GetRegister(rs).S32[ss] / RecompilationContext_GetRegister(rt).S32[ss]; \
        ctx->hi = RecompilationContext_GetRegister(rs).S32[ss] % RecompilationContext_GetRegister(rt).S32[ss]; \
    } \
    else \
    { \
        ctx->lo.S64[dd] = (RecompilationContext_GetRegister(rs).S32[ss] < 0) ? 1 : -1; \
        ctx->hi.S64[dd] = RecompilationContext_GetRegister(rs).S32[ss]; \
    }

#define __mips_asm_pdivw(rd, rs, rt)  __mips_asm__pdivw(0,0); __mips_asm__pdivw(1,2);
#define __mips_asm_pcpyld(rd, rs, rt) \
    RecompilationContext_GetRegister(rd).U64[0] = RecompilationContext_GetRegister(rt).U64[0]; \
    RecompilationContext_GetRegister(rd).U64[1] = RecompilationContext_GetRegister(rs).U64[0];

// 100
#define __mips_asm_pmaddh(rd, rs, rt) \
    ctx->lo.U32[0] = ctx->lo[0] + (s32)RecompilationContext_GetRegister(rs).S16[0] * (s32)RecompilationContext_GetRegister(rt).S16[0]; \
    ctx->lo.U32[1] = ctx->lo[1] + (s32)RecompilationContext_GetRegister(rs).S16[1] * (s32)RecompilationContext_GetRegister(rt).S16[1]; \
    ctx->hi.U32[0] = ctx->lo[2] + (s32)RecompilationContext_GetRegister(rs).S16[2] * (s32)RecompilationContext_GetRegister(rt).S16[2]; \
    ctx->hi.U32[1] = ctx->lo[3] + (s32)RecompilationContext_GetRegister(rs).S16[3] * (s32)RecompilationContext_GetRegister(rt).S16[3]; \
    ctx->lo.U32[2] = ctx->lo[4] + (s32)RecompilationContext_GetRegister(rs).S16[4] * (s32)RecompilationContext_GetRegister(rt).S16[4]; \
    ctx->lo.U32[3] = ctx->lo[5] + (s32)RecompilationContext_GetRegister(rs).S16[5] * (s32)RecompilationContext_GetRegister(rt).S16[5]; \
    ctx->hi.U32[2] = ctx->lo[6] + (s32)RecompilationContext_GetRegister(rs).S16[6] * (s32)RecompilationContext_GetRegister(rt).S16[6]; \
    ctx->hi.U32[3] = ctx->lo[7] + (s32)RecompilationContext_GetRegister(rs).S16[7] * (s32)RecompilationContext_GetRegister(rt).S16[7]; \
    RecompilationContext_GetRegister(rd).U32[0] = ctx->lo.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[1] = ctx->hi.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[2] = ctx->lo.U32[2]; \
    RecompilationContext_GetRegister(rd).U32[3] = ctx->hi.U32[2];

#define __mips_asm__phmadhlo(rd, rs, tr, dd, n) \
    s32 tmp = (s32)rs.S16[n + 1] * (s32)rt.S16[n + 1]; \
    s32 tmp2 = tmp + (s32)RecompilationContext_GetRegister(rs).S16[n] * (s32)RecompilationContext_GetRegister(rt).S16[n]; \
    ctx->lo.U32[dd] = tmp2; \
    ctx->lo.U32[dd + 1] = tmp;
#define __mips_asm__phmadhhi(rd, rs, tr, dd, n) \
    s32 tmp = (s32)rs.S16[n + 1] * (s32)rt.S16[n + 1]; \
    s32 tmp2 = tmp + (s32)RecompilationContext_GetRegister(rs).S16[n] * (s32)RecompilationContext_GetRegister(rt).S16[n]; \
    ctx->hi.U32[dd] = tmp2; \
    ctx->hi.U32[dd + 1] = tmp;
#define __mips_asm_phmadh(rd, rs, rt) \
    __mips_asm__phmadh_lo(rd, rs, rt, 0, 0); \
    __mips_asm__phmadh_hi(rd, rs, rt, 0, 2); \
    __mips_asm__phmadh_lo(rd, rs, rt, 2, 4); \
    __mips_asm__phmadh_hi(rd, rs, rt, 2, 6); \
    RecompilationContext_GetRegister(rd).u32[0] = ctx->lo.u32[0]; \
    RecompilationContext_GetRegister(rd).u32[1] = ctx->hi.u32[0]; \
    RecompilationContext_GetRegister(rd).u32[2] = ctx->lo.u32[2]; \
    RecompilationContext_GetRegister(rd).u32[3] = ctx->hi.u32[2];

#define __mips_asm_pand(rd, rt, rs) \
    RecompilationContext_GetRegister(rd).U64[0] = RecompilationContext_GetRegister(rs).U64[0] & RecompilationContext_GetRegister(rt).U64[0]; \
    RecompilationContext_GetRegister(rd).U64[1] = RecompilationContext_GetRegister(rs).U64[1] & RecompilationContext_GetRegister(rt).U64[1];
#define __mips_asm_pxor(rd, rt, rs) \
    RecompilationContext_GetRegister(rd).U64[0] = RecompilationContext_GetRegister(rs).U64[0] ^ RecompilationContext_GetRegister(rt).U64[0]; \
    RecompilationContext_GetRegister(rd).U64[1] = RecompilationContext_GetRegister(rs).U64[1] ^ RecompilationContext_GetRegister(rt).U64[1];

// 101
#define __mips_asm_pmsubh(rd, rs, rt) \
    ctx->lo.U32[0] = ctx->lo.U32[0] - (s32)RecompilationContext_GetRegister(rs).s16[0] * (s32)tr.S16[0]; \
    ctx->lo.U32[1] = ctx->lo.U32[1] - (s32)RecompilationContext_GetRegister(rs).s16[1] * (s32)tr.S16[1]; \
    ctx->lo.U32[0] = ctx->lo.U32[2] - (s32)RecompilationContext_GetRegister(rs).s16[2] * (s32)tr.S16[2]; \
    ctx->lo.U32[1] = ctx->lo.U32[3] - (s32)RecompilationContext_GetRegister(rs).s16[3] * (s32)tr.S16[3]; \
    ctx->lo.U32[2] = ctx->lo.U32[4] - (s32)RecompilationContext_GetRegister(rs).s16[4] * (s32)tr.S16[4]; \
    ctx->lo.U32[3] = ctx->lo.U32[5] - (s32)RecompilationContext_GetRegister(rs).s16[5] * (s32)tr.S16[5]; \
    ctx->lo.U32[2] = ctx->lo.U32[6] - (s32)RecompilationContext_GetRegister(rs).s16[6] * (s32)tr.S16[6]; \
    ctx->lo.U32[3] = ctx->lo.U32[7] - (s32)RecompilationContext_GetRegister(rs).s16[7] * (s32)tr.S16[7]; \
    RecompilationContext_GetRegister(rd).u32[0] = ctx->lo[0]; \
    RecompilationContext_GetRegister(rd).u32[1] = ctx->hi[0]; \
    RecompilationContext_GetRegister(rd).u32[2] = ctx->lo[2]; \
    RecompilationContext_GetRegister(rd).u32[3] = ctx->hi[2];

#define __mips_asm__phmsbh_lo(rs, rt, dd, n) \
    { \
        s32 tmp = (s32)rs.S16[n + 1] * (s32)rt.S16[n + 1]; \
        s32 tmp2 = tmp - (s32)RecompilationContext_GetRegister(rs).S16[n] * RecompilationContext_GetRegister(rt).S16[n]; \
        ctx->lo[dd] = tmp2; \
        ctx->lo[dd + 1] = ~tmp; \
    }
#define __mips_asm__phmsbh_hi(rs, rt, dd, n) \
    { \
        s32 tmp = (s32)rs.S16[n + 1] * (s32)rt.S16[n + 1]; \
        s32 tmp2 = tmp - (s32)RecompilationContext_GetRegister(rs).S16[n] * RecompilationContext_GetRegister(rt).S16[n]; \
        ctx->hi[dd] = tmp2; \
        ctx->hi[dd + 1] = ~tmp; \
    }
#define __mips_asm_phmsbh(rd, rs, rt) \
    __mips_asm__phmsbh_lo(rs, rt, 0, 0); \
    __mips_asm__phmsbh_hi(rs, rt, 0, 2); \
    __mips_asm__phmsbh_lo(rs, rt, 2, 4); \
    __mips_asm__phmsbh_hi(rs, rt, 2, 6); \
    RecompilationContext_GetRegister(rd).U32[0] = ctx->lo.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[1] = ctx->lo.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[2] = ctx->hi.U32[2]; \
    RecompilationContext_GetRegister(rd).U32[3] = ctx->hi.U32[2];

// 110
#define __mips_asm_pexeh(rd, rt) \
    RecompilationContext_GetRegister(rd).U16[0] = RecompilationContext_GetRegister(rt).U16[2]; \
    RecompilationContext_GetRegister(rd).U16[1] = RecompilationContext_GetRegister(rt).U16[1]; \
    RecompilationContext_GetRegister(rd).U16[2] = RecompilationContext_GetRegister(rt).U16[0]; \
    RecompilationContext_GetRegister(rd).U16[3] = RecompilationContext_GetRegister(rt).U16[3]; \
    RecompilationContext_GetRegister(rd).U16[4] = RecompilationContext_GetRegister(rt).U16[6]; \
    RecompilationContext_GetRegister(rd).U16[5] = RecompilationContext_GetRegister(rt).U16[5]; \
    RecompilationContext_GetRegister(rd).U16[6] = RecompilationContext_GetRegister(rt).U16[4]; \
    RecompilationContext_GetRegister(rd).U16[7] = RecompilationContext_GetRegister(rt).U16[7]; 
#define __mips_asm_prevh(rd, rt) \
    RecompilationContext_GetRegister(rd).U16[0] = RecompilationContext_GetRegister(rt).U16[3]; \
    RecompilationContext_GetRegister(rd).U16[1] = RecompilationContext_GetRegister(rt).U16[2]; \
    RecompilationContext_GetRegister(rd).U16[2] = RecompilationContext_GetRegister(rt).U16[1]; \
    RecompilationContext_GetRegister(rd).U16[3] = RecompilationContext_GetRegister(rt).U16[0]; \
    RecompilationContext_GetRegister(rd).U16[4] = RecompilationContext_GetRegister(rt).U16[7]; \
    RecompilationContext_GetRegister(rd).U16[5] = RecompilationContext_GetRegister(rt).U16[6]; \
    RecompilationContext_GetRegister(rd).U16[6] = RecompilationContext_GetRegister(rt).U16[5]; \
    RecompilationContext_GetRegister(rd).U16[7] = RecompilationContext_GetRegister(rt).U16[4];


// 111
#define __mips_asm_pmulth(rd, rs, rt) \
    ctx->lo.U32[0] = (s32)RecompilationContext_GetRegister(rs).S16[0] * (s32)RecompilationContext_GetRegister(rt).S16[0]; \
    ctx->lo.U32[1] = (s32)RecompilationContext_GetRegister(rs).S16[1] * (s32)RecompilationContext_GetRegister(rt).S16[1]; \
    ctx->hi.U32[0] = (s32)RecompilationContext_GetRegister(rs).S16[2] * (s32)RecompilationContext_GetRegister(rt).S16[2]; \
    ctx->hi.U32[1] = (s32)RecompilationContext_GetRegister(rs).S16[3] * (s32)RecompilationContext_GetRegister(rt).S16[3]; \
    ctx->lo.U32[2] = (s32)RecompilationContext_GetRegister(rs).S16[4] * (s32)RecompilationContext_GetRegister(rt).S16[4]; \
    ctx->lo.U32[3] = (s32)RecompilationContext_GetRegister(rs).S16[5] * (s32)RecompilationContext_GetRegister(rt).S16[5]; \
    ctx->hi.U32[2] = (s32)RecompilationContext_GetRegister(rs).S16[6] * (s32)RecompilationContext_GetRegister(rt).S16[6]; \
    ctx->hi.U32[3] = (s32)RecompilationContext_GetRegister(rs).S16[7] * (s32)RecompilationContext_GetRegister(rt).S16[7]; \
    RecompilationContext_GetRegister(rd).U32[0] = ctx->lo.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[1] = ctx->hi.U32[0]; \
    RecompilationContext_GetRegister(rd).U32[2] = ctx->lo.U32[2]; \
    RecompilationContext_GetRegister(rd).U32[3] = ctx->hi.U32[2]; \
        
#define __mips_asm__pdivbw(rs, rt, n) \
    if (RecompilationContext_GetRegister(rs).U32[n] == 0x80000000 && RecompilationContext_GetRegister(rt).U16[0] == 0xFFFF) \
    { \
        ctx->lo.S32[n] = (s32)0x80000000; \
        ctx->hi.S32[n] = (s32)0; \
    } \
    else if (RecompilationContext_GetRegister(rt).U32[0] != 0) \
    { \
        ctx->lo.S32[n] = RecompilationContext_GetRegister(rs).S32[n] / RecompilationContext_GetRegister(rt).S16[0]; \
        ctx->hi.S32[n] = RecompilationContext_GetRegister(rs).S32[n] % RecompilationContext_GetRegister(rt).S16[0]; \
    } else { \
        ctx->lo.S32[n] = (RecompilationContext_GetRegister(rs).S32[n] < 0) ? 1 : -1; \
        ctx->hi.S32[n] = RecompilationContext_GetRegister(rs).S32[n]; \
    }
#define __mips_asm_pdivbw(rs, rt) __mips_asm__pdivbw(rs, rt, 0); __mips_asm__pdivbw(rs, rt, 1); __mips_asm__pdivbw(rs, rt, 2); __mips_asm__pdivbw(rs, rt, 3); 
#define __mips_asm_pexew(rd, rt) \
    RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[2]; \
    RecompilationContext_GetRegister(rd).U32[1] = RecompilationContext_GetRegister(rt).U32[1]; \
    RecompilationContext_GetRegister(rd).U32[2] = RecompilationContext_GetRegister(rt).U32[0]; \
    RecompilationContext_GetRegister(rd).U32[3] = RecompilationContext_GetRegister(rt).U32[3];
#define __mips_asm_prot3w(rd, rt) \
    RecompilationContext_GetRegister(rd).U32[0] = RecompilationContext_GetRegister(rt).U32[1]; \
    RecompilationContext_GetRegister(rd).U32[1] = RecompilationContext_GetRegister(rt).U32[2]; \
    RecompilationContext_GetRegister(rd).U32[2] = RecompilationContext_GetRegister(rt).U32[0]; \
    RecompilationContext_GetRegister(rd).U32[3] = RecompilationContext_GetRegister(rt).U32[3];

// ===================================
// MMI3
// ===================================
// 000
#define __mips_asm_pmadduw(...)
// #define __mips_asm_psravw(...)

// 010
#define __mips_asm_pmthi(...)
#define __mips_asm_pmtlo(...)
#define __mips_asm_pinteh(...)

// 011
#define __mips_asm_pmultuw(...)
#define __mips_asm_pdivuw(...)
#define __mips_asm_pcpyud(...)

// 100
#define __mips_asm_por(...)
#define __mips_asm_pnor(...)

// 110
#define __mips_asm_pexch(...)
#define __mips_asm_pcpyh(...)

// 111
#define __mips_asm_pexcw(...)

// ===================================
// COP0
// ===================================
// 000
#define __mips_asm_mfc0(rt, rd) RecompilationContext_GetRegister(rt).U32[0] = rd;
#define __mips_asm_mtc0(rt, rd) rd = RecompilationContext_GetRegister(rt).U32[0];

// ===================================
// BC0
// ===================================
// 000
#define __mips_asm_bc0f(...)
#define __mips_asm_bc0t(...)
#define __mips_asm_bc0fl(...)
#define __mips_asm_bc0tl(...)

// ===================================
// TLB / Exception
// ===================================
// 000
#define __mips_asm_tlbr(...)
#define __mips_asm_tlbwi(...)
#define __mips_asm_tlbwr(...)

// 001
#define __mips_asm_tlbp(...)

// 011
#define __mips_asm_eret perror("Error return."); // unimplemented instruction

// 111
#define __mips_asm_ei ctx->Status |= (1 | (1 << 16)); 
#define __mips_asm_di ctx->Status &= ~(1 | (1 << 16));

// ===================================
// COP1
// ===================================
// 000
#define __mips_asm_mfc1(...)
#define __mips_asm_cfc1(...)
#define __mips_asm_mtc1(rt, fs) ctx->FPURegisters[fs] = *(float*)(&ctx->ee_registers[rt]);
#define __mips_asm_ctc1(...)

// ===================================
// BC1
// ===================================
// 000
#define __mips_asm_bc1f(branch, nextInstruction, i) if ((*(int*)(&ctx->f31) & (1 << 23)) == 0) { nextInstruction; goto branch; } if (false) { branch_##i: nextInstruction;}
#define __mips_asm_bc1t(...)
#define __mips_asm_bc1fl(...)
#define __mips_asm_bc1tl(...)

// ===================================
// FPU.S
// ===================================
// 000
#define __mips_asm_adds(fd, fs, ft) ctx->FPURegisters[fd] = (float)ctx->FPURegisters[fs] + (float)ctx->FPURegisters[ft];
#define __mips_asm_subs(...)
#define __mips_asm_muls(fd, fs, ft) ctx->FPURegisters[fd] = (float)ctx->FPURegisters[fs] * (float)ctx->FPURegisters[ft];
#define __mips_asm_divs(fd, fs, ft) ctx->FPURegisters[fd] = ctx->FPURegisters[fs] / ctx->FPURegisters[ft];
#define __mips_asm_sqrts(...)
#define __mips_asm_abss(fd, fs) ctx->FPURegisters[fs] = (float)(ctx->FPURegisters[fs] < 0 ? -ctx->FPURegisters[fs] : ctx->FPURegisters[fs]);
#define __mips_asm_movs(fd, fs) ctx->FPURegisters[fs] = ctx->FPURegisters[fs];
#define __mips_asm_negs(...)

// 010
#define __mips_asm_rsqrts(...)

// 011
#define __mips_asm_addas(...)
#define __mips_asm_subas(...)
#define __mips_asm_mulas(...)
#define __mips_asm_madds(...)
#define __mips_asm_msubs(...)
#define __mips_asm_maddas(...)
#define __mips_asm_msubas(...)

// 100
#define __mips_asm_cvtw(...)
#define __mips_asm_cvtws(...)

// 101
#define __mips_asm_maxs(...)
#define __mips_asm_mins(...)

// 110
#define __mips_asm_cf(...)
#define __mips_asm_ceq(...)
#define __mips_asm_ceqs(...)
#define __mips_asm_clt(...)
#define __mips_asm_clts(...)
#define __mips_asm_cle(...)
#define __mips_asm_cles(...)

#define __mips_asm_cvtsw(...) // technicall fpu.w but its the only instruction

// ===================================
// COP2
// ===================================
// 000
#define __mips_asm_qmfc2(...)
#define __mips_asm_cfc2(...)
#define __mips_asm_qmtc2(...)
#define __mips_asm_ctc2(...)

// I'm uncertain about how these work
#define __mips_asm_cfc2ni(...)
#define __mips_asm_ctc2ni(...)
#define __mips_asm_qmtc2ni(...)
#define __mips_asm_qmfc2ni(...)

// ===================================
// BC2
// ===================================
// 000
#define __mips_asm_bc2f(...)
#define __mips_asm_bc2t(...)
#define __mips_asm_bc2fl(...)
#define __mips_asm_bc2tl(...)

// ===================================
// COP2 Special 1
// ===================================
// 000
#define __mips_asm_vaddx(...)
#define __mips_asm_vaddy(...)
#define __mips_asm_vaddz(...)
#define __mips_asm_vaddw(...)
#define __mips_asm_vsubx(...)
#define __mips_asm_vsuby(...)
#define __mips_asm_vsubz(...)
#define __mips_asm_vsubw(...)

// 001
#define __mips_asm_vmaddx(...)
#define __mips_asm_vmaddy(...)
#define __mips_asm_vmaddz(...)
#define __mips_asm_vmaddw(...)
#define __mips_asm_vmsubx(...)
#define __mips_asm_vmsuby(...)
#define __mips_asm_vmsubz(...)
#define __mips_asm_vmsubw(...)

// 010
#define __mips_asm_vmaxx(...)
#define __mips_asm_vmaxy(...)
#define __mips_asm_vmaxz(...)
#define __mips_asm_vmaxw(...)
#define __mips_asm_vminix(...)
#define __mips_asm_vminiy(...)
#define __mips_asm_vminiz(...)
#define __mips_asm_vminiw(...)

// 011
#define __mips_asm_vmulx(...)
#define __mips_asm_vmuly(...)
#define __mips_asm_vmulz(...)
#define __mips_asm_vmulw(...)
#define __mips_asm_vmulq(...)
#define __mips_asm_vmaxi(...)
#define __mips_asm_vmuli(...)
#define __mips_asm_vminii(...)

// 100
#define __mips_asm_vaddq(...)
#define __mips_asm_vmaddq(...)
#define __mips_asm_vaddi(...)
#define __mips_asm_vmaddi(...)
#define __mips_asm_vsubq(...)
#define __mips_asm_vmsubq(...)
#define __mips_asm_vsubi(...)
#define __mips_asm_vmsubi(...)

// 101
#define __mips_asm_vadd(...)
#define __mips_asm_vmadd(...)
#define __mips_asm_vmul(...)
#define __mips_asm_vmax(...)
#define __mips_asm_vsub(...)
#define __mips_asm_vmsub(...)
#define __mips_asm_vopmsub(...)
#define __mips_asm_vmini(...)

// 110
#define __mips_asm_viadd(...)
#define __mips_asm_visub(...)
#define __mips_asm_viaddi(...)
#define __mips_asm_viand(...)
#define __mips_asm_vior(...)

// 111
#define __mips_asm_vcallms(...)
#define __mips_asm_callmsr(...)

// ===================================
// COP2 Special 2
// ===================================
// 000
#define __mips_asm_vaddax(...)
#define __mips_asm_vadday(...)
#define __mips_asm_vaddaz(...)
#define __mips_asm_vaddaw(...)
#define __mips_asm_vsubax(...)
#define __mips_asm_vsubay(...)
#define __mips_asm_vsubaz(...)
#define __mips_asm_vsubaw(...)

// 001
#define __mips_asm_vmaddax(...)
#define __mips_asm_vmadday(...)
#define __mips_asm_vmaddaz(...)
#define __mips_asm_vmaddaw(...)
#define __mips_asm_vmsubax(...)
#define __mips_asm_vmsubay(...)
#define __mips_asm_vmsubaz(...)
#define __mips_asm_vmsubaw(...)

// 010
#define __mips_asm_vitof0(...)
#define __mips_asm_vitof4(...)
#define __mips_asm_vitof12(...)
#define __mips_asm_vitof15(...)
#define __mips_asm_vftoi0(...)
#define __mips_asm_vftoi4(...)
#define __mips_asm_vftoi12(...)
#define __mips_asm_vftoi15(...)

// 011
#define __mips_asm_vmulax(...)
#define __mips_asm_vmulay(...)
#define __mips_asm_vmulaz(...)
#define __mips_asm_vmulaw(...)
#define __mips_asm_vmulaq(...)
#define __mips_asm_vabs(...)
#define __mips_asm_vmulai(...)
#define __mips_asm_vclipw(...)

// 100
#define __mips_asm_vaddaq(...)
#define __mips_asm_vmaddaq(...)
#define __mips_asm_vaddai(...)
#define __mips_asm_vmaddai(...)
#define __mips_asm_vsubaq(...)
#define __mips_asm_vmsubaq(...)
#define __mips_asm_vsubai(...)
#define __mips_asm_vmsubai(...)

// 101
#define __mips_asm_vadda(...)
#define __mips_asm_vmadda(...)
#define __mips_asm_vmula(...)
#define __mips_asm_vsuba(...)
#define __mips_asm_vmsuba(...)
#define __mips_asm_vopmula(...)
#define __mips_asm_vnop

// 110
#define __mips_asm_vmove(...)
#define __mips_asm_vmr32(...)
#define __mips_asm_vlqui(...)
#define __mips_asm_vlqi(...)
#define __mips_asm_vsqi(...)
#define __mips_asm_vlqd(...)
#define __mips_asm_vsqd(...)

// 111
#define __mips_asm_vdiv(...)
#define __mips_asm_vsqrt(...)
#define __mips_asm_vrsqrt(...)
#define __mips_asm_vwaitq
#define __mips_asm_vwaitq
#define __mips_asm_vmtir(...)
#define __mips_asm_vmfir(...)
#define __mips_asm_vilwr(...)
#define __mips_asm_viswr(...)

// 1000
#define __mips_asm_vrnext(...)
#define __mips_asm_vrnext(...)
#define __mips_asm_vrget(...)
#define __mips_asm_vrinit(...)
#define __mips_asm_vrxor(...)


// ===================================
// Instruction Aliases
// ===================================
#define __mips_asm_b(branch, nextInstruction, i) nextInstruction; goto branch; branch_##i: nextInstruction;
#define __mips_asm_bnez(rs, branch, nextInstruction, i) if (RecompilationContext_GetRegister(rs).S32[0] != 0) { nextInstruction; goto branch; } branch_##i: nextInstruction;
#define __mips_asm_beqz(rs, branch, nextInstruction, i) if (RecompilationContext_GetRegister(rs).S32[0] == 0) { nextInstruction; goto branch; } branch_##i: nextInstruction;

#define __mips_asm_negu(rd, rs) RecompilationContext_GetRegister(rd).U64[0] = -RecompilationContext_GetRegister(rs).U64[0];

#define __mips_asm_ret(NextInstruction) NextInstruction; return;
#define __mips_asm_ret_value(NextInstruction, value) NextInstruction; return value;

#define __mips_asm_syncp
#define __mips_asm_c1(...)

#define __mips_asm__jalr(address, nextInstruction) nextInstruction; jalr(address);

#endif


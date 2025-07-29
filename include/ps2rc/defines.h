#pragma once

#define true 1
#define false 0

typedef unsigned long u64; 
typedef unsigned int u32; 
typedef unsigned short u16; 
typedef unsigned char u8; 

typedef long s64; 
typedef int s32; 
typedef short s16; 
typedef char s8; 

typedef u8 b8;
typedef u32 b32;

typedef s32 undefined;
typedef s8 undefined1;
typedef s16 undefined2;
typedef s32 undefined4;

typedef struct s128 {
    s64 high;
    s64 low;
} s128;

typedef struct u128 {
    u64 high;
    u64 low;
} u128;

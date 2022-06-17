#pragma once

// Unsigned int types.
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

// Signed int types.
typedef signed char      s8;
typedef signed short     s16;
typedef signed int       s32;
typedef signed long long s64;

// Floating point types
typedef float  f32;
typedef double f64;

#include <float.h>
#define F32Max FLT_MAX
#define F32Min FLT_MIN

#define DebugLog(...)                        \
    {                                                                   \
        char temp[200] = {};                                            \
        DWORD Size = sprintf_s(temp, __VA_ARGS__);                        \
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), temp, Size, NULL, NULL); \
    }                                               

#define WarnIfNot(a)                                          \
    {                                                         \
        if(!(a)) {                                            \
            DebugLog(#a);                                     \
            DebugLog(" is false\n");                          \
        }                                                     \
    }                                                         \

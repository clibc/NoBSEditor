#pragma once

#include "stdint.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef float  f32;
typedef double f64;

#include <float.h>
#define F32Max FLT_MAX
#define F32Min FLT_MIN

#include <iostream>
#define DebugLog(...) printf(__VA_ARGS__)

#define WarnIfNot(a)                                          \
    {                                                         \
        if(!(a)) {                                            \
            DebugLog(#a);                                     \
            DebugLog(" is false\n");                          \
        }                                                     \
    }                                                         \

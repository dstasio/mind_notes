#if !defined(FI_MATH_H)
#include "datatypes.h"

ds_internal inline s32 min(s32 a, s32 b) { return ((a < b) ? a : b); }
ds_internal inline s32 max(s32 a, s32 b) { return ((a > b) ? a : b); }

ds_internal inline r32 lerp(r32 a, r32 b, r32 t)
{
    r32 result = (1.f - t)*a + t*b;
    return result;
}

#define FI_MATH_H
#endif

#if !defined(FI_MATH_H)
#include "datatypes.h"

internal inline s32 min(s32 a, s32 b) { return ((a < b) ? a : b); }
internal inline s32 max(s32 a, s32 b) { return ((a > b) ? a : b); }

#define FI_MATH_H
#endif

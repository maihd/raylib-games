#pragma once

#ifndef __cpluscplus
#error "MathOps.h require C++ for reloading operators."
#endif

#define RAYMATH_HEADER_ONLY
#define RAYMATH_STANDALONE
#include <raymath.h>

RMDEF Vector2 operator+(Vector2 a, Vector2 b)
{
    return Vector2Add(a, b);
}

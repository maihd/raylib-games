#pragma once

#include "MaiTypes.h"           // Required for structs: vec3, mat4

#define MAIMATH_DEF static inline // Functions may be inlined, no external out-of-line definition

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef PI
#   define PI 3.14159265358979323846
#endif

#ifndef DEG2RAD
#   define DEG2RAD (PI/180.0f)
#endif

#ifndef RAD2DEG
#   define RAD2DEG (180.0f/PI)
#endif

// Return float vector for mat4
#ifndef mat4ToFloat
#   define mat4ToFloat(mat) (mat4ToFloatV(mat).v)
#endif

// Return float vector for vec3
#ifndef vec3ToFloat
#   define vec3ToFloat(vec) (vec3ToFloatV(vec).v)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// NOTE: Helper types to be used instead of array return types for *ToFloat functions
typedef struct float3 { float v[3]; } float3;
typedef struct float16 { float v[16]; } float16;

#include <math.h>       // Required for: sinf(), cosf(), tan(), fabs()

//----------------------------------------------------------------------------------
// Module Functions Definition - Utils math
//----------------------------------------------------------------------------------

// Clamp float value
MAIMATH_DEF float clampf(float value, float min, float max)
{
    const float res = value < min ? min : value;
    return res > max ? max : res;
}

// Calculate linear interpolation between two floats
MAIMATH_DEF float lerpf(float start, float end, float amount)
{
    return start + amount * (end - start);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - vec2 math
//----------------------------------------------------------------------------------

// Vector with components value 0.0f
MAIMATH_DEF vec2 vec2Zero(void)
{
    vec2 result = { 0.0f, 0.0f };
    return result;
}

// Vector with components value 1.0f
MAIMATH_DEF vec2 vec2One(void)
{
    vec2 result = { 1.0f, 1.0f };
    return result;
}

MAIMATH_DEF vec2 vec2From(float x, float y)
{
    vec2 result = { x, y };
    return result;
}

MAIMATH_DEF vec2 vec2Repeat(float s)
{
    vec2 result = { s, s };
    return result;
}

// Negate vector
MAIMATH_DEF vec2 vec2Neg(vec2 v)
{
    vec2 result = { -v.x, -v.y };
    return result;
}

// Add two vectors (v1 + v2)
MAIMATH_DEF vec2 vec2Add(vec2 v1, vec2 v2)
{
    vec2 result = { v1.x + v2.x, v1.y + v2.y };
    return result;
}

// Subtract two vectors (v1 - v2)
MAIMATH_DEF vec2 vec2Sub(vec2 v1, vec2 v2)
{
    vec2 result = { v1.x - v2.x, v1.y - v2.y };
    return result;
}

// Multiply vector by vector
MAIMATH_DEF vec2 vec2Mul(vec2 v1, vec2 v2)
{
    vec2 result = { v1.x * v2.x, v1.y * v2.y };
    return result;
}

// Divide vector by vector
MAIMATH_DEF vec2 vec2Div(vec2 v1, vec2 v2)
{
    vec2 result = { v1.x / v2.x, v1.y / v2.y };
    return result;
}

// Calculate two vectors dot product
MAIMATH_DEF float vec2Dot(vec2 v1, vec2 v2)
{
    float result = v1.x * v2.x + v1.y * v2.y;
    return result;
}

// Calculate vector length
MAIMATH_DEF float vec2Length(vec2 v)
{
    float result = sqrtf(v.x * v.x + v.y * v.y);
    return result;
}

// Calculate vector squared length
MAIMATH_DEF float vec2LengthSq(vec2 v)
{
    float result = v.x * v.x + v.y * v.y;
    return result;
}

// Calculate distance between two vectors
MAIMATH_DEF float vec2Distance(vec2 v1, vec2 v2)
{
    float result = sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));
    return result;
}

// Calculate distance between two vectors
MAIMATH_DEF float vec2DistanceSq(vec2 v1, vec2 v2)
{
    float result = (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y);
    return result;
}

// Calculate angle from two vectors in X-axis
MAIMATH_DEF float vec2Angle(vec2 v1, vec2 v2)
{
    float result = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0f / PI);
    if (result < 0) result += 360.0f;
    return result;
}

// Scale vector (multiply by value)
MAIMATH_DEF vec2 vec2Scale(vec2 v, float scale)
{
    vec2 result = { v.x*scale, v.y*scale };
    return result;
}

// Scale vector (multiply by value)
MAIMATH_DEF vec2 vec2ScaleInv(vec2 v, float scale)
{
    vec2 result = { v.x / scale, v.y / scale };
    return result;
}

// Normalize provided vector
MAIMATH_DEF vec2 vec2Normalize(vec2 v)
{
    float length = vec2Length(v);
    if (length == 0.0f)
    {
        return v;
    }

    vec2 result = vec2Scale(v, 1.0f / vec2Length(v));
    return result;
}

// Calculate linear interpolation between two vectors
MAIMATH_DEF vec2 vec2Lerp(vec2 v1, vec2 v2, float amount)
{
    vec2 result = { 0 };

    result.x = v1.x + amount*(v2.x - v1.x);
    result.y = v1.y + amount*(v2.y - v1.y);

    return result;
}

// Calculate linear interpolation between two vectors
MAIMATH_DEF vec2 vec2Clamp(vec2 v, vec2 min, vec2 max)
{
    vec2 result = { 0 };

    result.x = clampf(v.x, min.x, max.x);
    result.y = clampf(v.y, min.y, max.y);

    return result;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - vec3 math
//----------------------------------------------------------------------------------

// Vector with components value 0.0f
MAIMATH_DEF vec3 vec3Zero(void)
{
    vec3 result = { 0.0f, 0.0f, 0.0f };
    return result;
}

// Vector with components value 1.0f
MAIMATH_DEF vec3 vec3One(void)
{
    vec3 result = { 1.0f, 1.0f, 1.0f };
    return result;
}

MAIMATH_DEF vec3 vec3From(float x, float y, float z)
{
    vec3 result = { x, y, z };
    return result;
}

MAIMATH_DEF vec3 vec3Repeat(float s)
{
    vec3 result = { s, s, s };
    return result;
}

// Add two vectors
MAIMATH_DEF vec3 vec3Add(vec3 v1, vec3 v2)
{
    vec3 result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    return result;
}

// Subtract two vectors
MAIMATH_DEF vec3 vec3Sub(vec3 v1, vec3 v2)
{
    vec3 result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    return result;
}

// Multiply vector by vector
MAIMATH_DEF vec3 vec3Mul(vec3 v1, vec3 v2)
{
    vec3 result = { v1.x*v2.x, v1.y*v2.y, v1.z*v2.z };
    return result;
}

// Calculate two vectors cross product
MAIMATH_DEF vec3 vec3Cross(vec3 v1, vec3 v2)
{
    vec3 result = { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
    return result;
}

// Calculate one vector perpendicular vector
MAIMATH_DEF vec3 vec3Perpendicular(vec3 v)
{
    vec3 result = { 0 };

    float min = (float) fabs(v.x);
    vec3 cardinalAxis = {1.0f, 0.0f, 0.0f};

    if (fabs(v.y) < min)
    {
        min = (float) fabs(v.y);
        vec3 tmp = {0.0f, 1.0f, 0.0f};
        cardinalAxis = tmp;
    }

    if (fabs(v.z) < min)
    {
        vec3 tmp = {0.0f, 0.0f, 1.0f};
        cardinalAxis = tmp;
    }

    result = vec3Cross(v, cardinalAxis);

    return result;
}

// Calculate vector length
MAIMATH_DEF float vec3Length(const vec3 v)
{
    float result = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    return result;
}

// Calculate vector length
MAIMATH_DEF float vec3LengthSq(const vec3 v)
{
    float result = v.x * v.x + v.y * v.y + v.z * v.z;
    return result;
}

// Calculate two vectors dot product
MAIMATH_DEF float vec3Dot(vec3 v1, vec3 v2)
{
    float result = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    return result;
}

// Calculate distance between two vectors
MAIMATH_DEF float vec3Distance(vec3 v1, vec3 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    float result = sqrtf(dx*dx + dy*dy + dz*dz);
    return result;
}

// Calculate distance between two vectors
MAIMATH_DEF float vec3DistanceSq(vec3 v1, vec3 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    float result = dx*dx + dy * dy + dz * dz;
    return result;
}

// Scale provided vector
MAIMATH_DEF vec3 vec3Scale(vec3 v, float scale)
{
    vec3 result = { v.x*scale, v.y*scale, v.z*scale };
    return result;
}

// Negate provided vector (invert direction)
MAIMATH_DEF vec3 vec3Negate(vec3 v)
{
    vec3 result = { -v.x, -v.y, -v.z };
    return result;
}

// Divide vector by a float value
MAIMATH_DEF vec3 vec3Div(vec3 v, float div)
{
    vec3 result = { v.x / div, v.y / div, v.z / div };
    return result;
}

// Divide vector by vector
MAIMATH_DEF vec3 vec3DivV(vec3 v1, vec3 v2)
{
    vec3 result = { v1.x/v2.x, v1.y/v2.y, v1.z/v2.z };
    return result;
}

// Normalize provided vector
MAIMATH_DEF vec3 vec3Normalize(vec3 v)
{
    vec3 result = v;

    float length, ilength;
    length = vec3Length(v);
    if (length == 0.0f) length = 1.0f;
    ilength = 1.0f/length;

    result.x *= ilength;
    result.y *= ilength;
    result.z *= ilength;

    return result;
}

// Orthonormalize provided vectors
// Makes vectors normalized and orthogonal to each other
// Gram-Schmidt function implementation
MAIMATH_DEF void vec3OrthoNormalize(vec3 *v1, vec3 *v2)
{
    *v1 = vec3Normalize(*v1);
    vec3 vn = vec3Cross(*v1, *v2);
    vn = vec3Normalize(vn);
    *v2 = vec3Cross(vn, *v1);
}

// Transforms a vec3 by a given mat4
MAIMATH_DEF vec3 vec3Transform(vec3 v, mat4 mat)
{
    vec3 result = { 0 };
    float x = v.x;
    float y = v.y;
    float z = v.z;

    result.x = mat.m0*x + mat.m4*y + mat.m8*z + mat.m12;
    result.y = mat.m1*x + mat.m5*y + mat.m9*z + mat.m13;
    result.z = mat.m2*x + mat.m6*y + mat.m10*z + mat.m14;

    return result;
}

// Transform a vector by quaternion rotation
MAIMATH_DEF vec3 vec3RotateByquat(vec3 v, quat q)
{
    vec3 result = { 0 };

    result.x = v.x*(q.x*q.x + q.w*q.w - q.y*q.y - q.z*q.z) + v.y*(2*q.x*q.y - 2*q.w*q.z) + v.z*(2*q.x*q.z + 2*q.w*q.y);
    result.y = v.x*(2*q.w*q.z + 2*q.x*q.y) + v.y*(q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z) + v.z*(-2*q.w*q.x + 2*q.y*q.z);
    result.z = v.x*(-2*q.w*q.y + 2*q.x*q.z) + v.y*(2*q.w*q.x + 2*q.y*q.z)+ v.z*(q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);

    return result;
}

// Calculate linear interpolation between two vectors
MAIMATH_DEF vec3 vec3Lerp(vec3 v1, vec3 v2, float amount)
{
    vec3 result = { 0 };

    result.x = v1.x + amount*(v2.x - v1.x);
    result.y = v1.y + amount*(v2.y - v1.y);
    result.z = v1.z + amount*(v2.z - v1.z);

    return result;
}

// Calculate reflected vector to normal
MAIMATH_DEF vec3 vec3Reflect(vec3 v, vec3 normal)
{
    // I is the original vector
    // N is the normal of the incident plane
    // R = I - (2*N*( DotProduct[ I,N] ))

    vec3 result = { 0 };

    float dotProduct = vec3DotProduct(v, normal);

    result.x = v.x - (2.0f*normal.x)*dotProduct;
    result.y = v.y - (2.0f*normal.y)*dotProduct;
    result.z = v.z - (2.0f*normal.z)*dotProduct;

    return result;
}

// Return min value for each pair of components
MAIMATH_DEF vec3 vec3Min(vec3 v1, vec3 v2)
{
    vec3 result = { 0 };

    result.x = fminf(v1.x, v2.x);
    result.y = fminf(v1.y, v2.y);
    result.z = fminf(v1.z, v2.z);

    return result;
}

// Return max value for each pair of components
MAIMATH_DEF vec3 vec3Max(vec3 v1, vec3 v2)
{
    vec3 result = { 0 };

    result.x = fmaxf(v1.x, v2.x);
    result.y = fmaxf(v1.y, v2.y);
    result.z = fmaxf(v1.z, v2.z);

    return result;
}

// Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c)
// NOTE: Assumes P is on the plane of the triangle
MAIMATH_DEF vec3 vec3Barycenter(vec3 p, vec3 a, vec3 b, vec3 c)
{
    //Vector v0 = b - a, v1 = c - a, v2 = p - a;

    vec3 v0 = vec3Sub(b, a);
    vec3 v1 = vec3Sub(c, a);
    vec3 v2 = vec3Sub(p, a);
    float d00 = vec3Dot(v0, v0);
    float d01 = vec3Dot(v0, v1);
    float d11 = vec3Dot(v1, v1);
    float d20 = vec3Dot(v2, v0);
    float d21 = vec3Dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    float idenom = 1.0f / denom;

    vec3 result = { 
        .x = (d11 * d20 - d01 * d21) * idenom, 
        .y = (d00 * d21 - d01 * d20) * idenom, 
        .z = 1.0f - (result.z + result.y) 
    };

    return result;
}

// Returns vec3 as float array
MAIMATH_DEF float3 vec3ToFloatV(vec3 v)
{
    float3 buffer = { 0 };

    buffer.v[0] = v.x;
    buffer.v[1] = v.y;
    buffer.v[2] = v.z;

    return buffer;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - vec4 math
//----------------------------------------------------------------------------------

// Vector with components value 0.0f
MAIMATH_DEF vec4 vec4Zero(void)
{
    vec4 result = { 0.0f, 0.0f, 0.0f, 0.0f };
    return result;
}

// Vector with components value 1.0f
MAIMATH_DEF vec4 vec4One(void)
{
    vec4 result = { 1.0f, 1.0f, 1.0f, 1.0f };
    return result;
}

MAIMATH_DEF vec4 vec4From(float x, float y, float z, float w)
{
    vec4 result = { x, y, z, w };
    return result;
}

MAIMATH_DEF vec4 vec4Repeat(float s)
{
    vec4 result = { s, s, s, s };
    return result;
}

// Add two vectors
MAIMATH_DEF vec4 vec4Add(vec4 v1, vec4 v2)
{
    vec4 result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    return result;
}

// Subtract two vectors
MAIMATH_DEF vec4 vec4Sub(vec4 v1, vec4 v2)
{
    vec4 result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
    return result;
}

// Multiply vector by vector
MAIMATH_DEF vec4 vec4Mul(vec4 v1, vec4 v2)
{
    vec4 result = { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w };
    return result;
}

// Calculate vector length
MAIMATH_DEF float vec4Length(const vec4 v)
{
    float result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    return result;
}

// Calculate vector length
MAIMATH_DEF float vec4LengthSq(const vec4 v)
{
    float result = v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    return result;
}

// Calculate two vectors dot product
MAIMATH_DEF float vec4Dot(vec4 v1, vec4 v2)
{
    float result = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
    return result;
}

// Calculate distance between two vectors
MAIMATH_DEF float vec4Distance(vec4 v1, vec4 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    float dw = v2.w - v1.w;
    float result = sqrtf(dx*dx + dy * dy + dz * dz + dw * dw);
    return result;
}

// Calculate distance between two vectors
MAIMATH_DEF float vec4DistanceSq(vec4 v1, vec4 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    float dw = v2.w - v1.w;
    float result = dx * dx + dy * dy + dz * dz + dw * dw;
    return result;
}

// Scale provided vector
MAIMATH_DEF vec4 vec4Scale(vec4 v, float scale)
{
    vec4 result = { v.x * scale, v.y * scale, v.z * scale, v.w * scale };
    return result;
}

// Scale provided vector
MAIMATH_DEF vec4 vec4ScaleInv(vec4 v, float scale)
{
    vec4 result = { v.x / scale, v.y / scale, v.z / scale, v.w / scale };
    return result;
}

// Negate provided vector (invert direction)
MAIMATH_DEF vec4 vec4Neg(vec4 v)
{
    vec4 result = { -v.x, -v.y, -v.z, -v.w };
    return result;
}

// Divide vector by vector
MAIMATH_DEF vec4 vec4Div(vec4 v1, vec4 v2)
{
    vec4 result = { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w };
    return result;
}

// Normalize provided vector
MAIMATH_DEF vec4 vec4Normalize(vec4 v)
{
    vec4 result = v;

    float length, ilength;
    length = vec4Length(v);
    if (length == 0.0f) length = 1.0f;
    ilength = 1.0f / length;

    result.x *= ilength;
    result.y *= ilength;
    result.z *= ilength;
    result.w *= ilength;

    return result;
}

// Transforms a vec4 by a given mat4
MAIMATH_DEF vec4 vec4Transform(vec4 v, mat4 mat)
{
    vec4 result = { 0 };
    float x = v.x;
    float y = v.y;
    float z = v.z;
    float w = v.w;

    result.x = mat.m0 * x + mat.m4 * y + mat.m8 * z + mat.m12 * w;
    result.y = mat.m1 * x + mat.m5 * y + mat.m9 * z + mat.m13 * w;
    result.z = mat.m2 * x + mat.m6 * y + mat.m10 * z + mat.m14 * w;
    result.w = mat.m3 * x + mat.m7 * y + mat.m11 * z + mat.m15 * w;

    return result;
}

// Calculate linear interpolation between two vectors
MAIMATH_DEF vec4 vec4Lerp(vec4 v1, vec4 v2, float amount)
{
    vec4 result = { 
        .x = v1.x + amount * (v2.x - v1.x),
        .y = v1.y + amount * (v2.y - v1.y),
        .z = v1.z + amount * (v2.z - v1.z),
        .w = v1.w + amount * (v2.w - v1.w),
    };

    return result;
}

// Return min value for each pair of components
MAIMATH_DEF vec4 vec4Min(vec4 v1, vec4 v2)
{
    vec4 result = { 
        .x = fminf(v1.x, v2.x),
        .y = fminf(v1.y, v2.y),
        .z = fminf(v1.z, v2.z),
        .w = fminf(v1.w, v2.w),
    };

    return result;
}

// Return max value for each pair of components
MAIMATH_DEF vec4 vec4Max(vec4 v1, vec4 v2)
{
    vec4 result = { 
        .x = fmaxf(v1.x, v2.x),
        .y = fmaxf(v1.y, v2.y),
        .z = fmaxf(v1.z, v2.z),
        .w = fmaxf(v1.w, v2.w),
    };

    return result;
}

// Return max value for each pair of components
MAIMATH_DEF vec4 vec4Clamp(vec4 v, vec4 min, vec4 max)
{
    vec4 result = { 
        .x = clampf(v.x, min.x, max.x),
        .y = clampf(v.y, min.y, max.y),
        .z = clampf(v.z, min.z, max.z),
        .w = clampf(v.w, min.w, max.w),
    };

    return result;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - mat4 math
//----------------------------------------------------------------------------------

// Compute matrix determinant
MAIMATH_DEF float mat4Determinant(mat4 mat)
{
    float result = { 0 };

    // Cache the matrix values (speed optimization)
    float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
    float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
    float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
    float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

    result = a30*a21*a12*a03 - a20*a31*a12*a03 - a30*a11*a22*a03 + a10*a31*a22*a03 +
             a20*a11*a32*a03 - a10*a21*a32*a03 - a30*a21*a02*a13 + a20*a31*a02*a13 +
             a30*a01*a22*a13 - a00*a31*a22*a13 - a20*a01*a32*a13 + a00*a21*a32*a13 +
             a30*a11*a02*a23 - a10*a31*a02*a23 - a30*a01*a12*a23 + a00*a31*a12*a23 +
             a10*a01*a32*a23 - a00*a11*a32*a23 - a20*a11*a02*a33 + a10*a21*a02*a33 +
             a20*a01*a12*a33 - a00*a21*a12*a33 - a10*a01*a22*a33 + a00*a11*a22*a33;

    return result;
}

// Returns the trace of the matrix (sum of the values along the diagonal)
MAIMATH_DEF float mat4Trace(mat4 mat)
{
    float result = (mat.m0 + mat.m5 + mat.m10 + mat.m15);
    return result;
}

// Transposes provided matrix
MAIMATH_DEF mat4 mat4Transpose(mat4 mat)
{
    mat4 result = { 0 };

    result.m0 = mat.m0;
    result.m1 = mat.m4;
    result.m2 = mat.m8;
    result.m3 = mat.m12;
    result.m4 = mat.m1;
    result.m5 = mat.m5;
    result.m6 = mat.m9;
    result.m7 = mat.m13;
    result.m8 = mat.m2;
    result.m9 = mat.m6;
    result.m10 = mat.m10;
    result.m11 = mat.m14;
    result.m12 = mat.m3;
    result.m13 = mat.m7;
    result.m14 = mat.m11;
    result.m15 = mat.m15;

    return result;
}

// Invert provided matrix
MAIMATH_DEF mat4 mat4Invert(mat4 mat)
{
    mat4 result = { 0 };

    // Cache the matrix values (speed optimization)
    float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
    float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
    float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
    float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

    float b00 = a00*a11 - a01*a10;
    float b01 = a00*a12 - a02*a10;
    float b02 = a00*a13 - a03*a10;
    float b03 = a01*a12 - a02*a11;
    float b04 = a01*a13 - a03*a11;
    float b05 = a02*a13 - a03*a12;
    float b06 = a20*a31 - a21*a30;
    float b07 = a20*a32 - a22*a30;
    float b08 = a20*a33 - a23*a30;
    float b09 = a21*a32 - a22*a31;
    float b10 = a21*a33 - a23*a31;
    float b11 = a22*a33 - a23*a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

    result.m0 = (a11*b11 - a12*b10 + a13*b09)*invDet;
    result.m1 = (-a01*b11 + a02*b10 - a03*b09)*invDet;
    result.m2 = (a31*b05 - a32*b04 + a33*b03)*invDet;
    result.m3 = (-a21*b05 + a22*b04 - a23*b03)*invDet;
    result.m4 = (-a10*b11 + a12*b08 - a13*b07)*invDet;
    result.m5 = (a00*b11 - a02*b08 + a03*b07)*invDet;
    result.m6 = (-a30*b05 + a32*b02 - a33*b01)*invDet;
    result.m7 = (a20*b05 - a22*b02 + a23*b01)*invDet;
    result.m8 = (a10*b10 - a11*b08 + a13*b06)*invDet;
    result.m9 = (-a00*b10 + a01*b08 - a03*b06)*invDet;
    result.m10 = (a30*b04 - a31*b02 + a33*b00)*invDet;
    result.m11 = (-a20*b04 + a21*b02 - a23*b00)*invDet;
    result.m12 = (-a10*b09 + a11*b07 - a12*b06)*invDet;
    result.m13 = (a00*b09 - a01*b07 + a02*b06)*invDet;
    result.m14 = (-a30*b03 + a31*b01 - a32*b00)*invDet;
    result.m15 = (a20*b03 - a21*b01 + a22*b00)*invDet;

    return result;
}

// Normalize provided matrix
MAIMATH_DEF mat4 mat4Normalize(mat4 mat)
{
    mat4 result = { 0 };

    float det = mat4Determinant(mat);

    result.m0 = mat.m0/det;
    result.m1 = mat.m1/det;
    result.m2 = mat.m2/det;
    result.m3 = mat.m3/det;
    result.m4 = mat.m4/det;
    result.m5 = mat.m5/det;
    result.m6 = mat.m6/det;
    result.m7 = mat.m7/det;
    result.m8 = mat.m8/det;
    result.m9 = mat.m9/det;
    result.m10 = mat.m10/det;
    result.m11 = mat.m11/det;
    result.m12 = mat.m12/det;
    result.m13 = mat.m13/det;
    result.m14 = mat.m14/det;
    result.m15 = mat.m15/det;

    return result;
}

// Returns identity matrix
MAIMATH_DEF mat4 mat4Identity(void)
{
    mat4 result = { 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f };

    return result;
}

// Add two matrices
MAIMATH_DEF mat4 mat4Add(mat4 left, mat4 right)
{
    mat4 result = mat4Identity();

    result.m0 = left.m0 + right.m0;
    result.m1 = left.m1 + right.m1;
    result.m2 = left.m2 + right.m2;
    result.m3 = left.m3 + right.m3;
    result.m4 = left.m4 + right.m4;
    result.m5 = left.m5 + right.m5;
    result.m6 = left.m6 + right.m6;
    result.m7 = left.m7 + right.m7;
    result.m8 = left.m8 + right.m8;
    result.m9 = left.m9 + right.m9;
    result.m10 = left.m10 + right.m10;
    result.m11 = left.m11 + right.m11;
    result.m12 = left.m12 + right.m12;
    result.m13 = left.m13 + right.m13;
    result.m14 = left.m14 + right.m14;
    result.m15 = left.m15 + right.m15;

    return result;
}

// Subtract two matrices (left - right)
MAIMATH_DEF mat4 mat4Subtract(mat4 left, mat4 right)
{
    mat4 result = mat4Identity();

    result.m0 = left.m0 - right.m0;
    result.m1 = left.m1 - right.m1;
    result.m2 = left.m2 - right.m2;
    result.m3 = left.m3 - right.m3;
    result.m4 = left.m4 - right.m4;
    result.m5 = left.m5 - right.m5;
    result.m6 = left.m6 - right.m6;
    result.m7 = left.m7 - right.m7;
    result.m8 = left.m8 - right.m8;
    result.m9 = left.m9 - right.m9;
    result.m10 = left.m10 - right.m10;
    result.m11 = left.m11 - right.m11;
    result.m12 = left.m12 - right.m12;
    result.m13 = left.m13 - right.m13;
    result.m14 = left.m14 - right.m14;
    result.m15 = left.m15 - right.m15;

    return result;
}

// Returns translation matrix
MAIMATH_DEF mat4 mat4Translate(float x, float y, float z)
{
    mat4 result = { 1.0f, 0.0f, 0.0f, x,
                      0.0f, 1.0f, 0.0f, y,
                      0.0f, 0.0f, 1.0f, z,
                      0.0f, 0.0f, 0.0f, 1.0f };

    return result;
}

// Create rotation matrix from axis and angle
// NOTE: Angle should be provided in radians
MAIMATH_DEF mat4 mat4Rotate(vec3 axis, float angle)
{
    mat4 result = { 0 };

    float x = axis.x, y = axis.y, z = axis.z;

    float length = sqrtf(x*x + y*y + z*z);

    if ((length != 1.0f) && (length != 0.0f))
    {
        length = 1.0f/length;
        x *= length;
        y *= length;
        z *= length;
    }

    float sinres = sinf(angle);
    float cosres = cosf(angle);
    float t = 1.0f - cosres;

    result.m0  = x*x*t + cosres;
    result.m1  = y*x*t + z*sinres;
    result.m2  = z*x*t - y*sinres;
    result.m3  = 0.0f;

    result.m4  = x*y*t - z*sinres;
    result.m5  = y*y*t + cosres;
    result.m6  = z*y*t + x*sinres;
    result.m7  = 0.0f;

    result.m8  = x*z*t + y*sinres;
    result.m9  = y*z*t - x*sinres;
    result.m10 = z*z*t + cosres;
    result.m11 = 0.0f;

    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

// Returns xyz-rotation matrix (angles in radians)
MAIMATH_DEF mat4 mat4RotateXYZ(vec3 ang)
{
    mat4 result = mat4Identity();

    float cosz = cosf(-ang.z);
    float sinz = sinf(-ang.z);
    float cosy = cosf(-ang.y);
    float siny = sinf(-ang.y);
    float cosx = cosf(-ang.x);
    float sinx = sinf(-ang.x);

    result.m0 = cosz * cosy;
    result.m4 = (cosz * siny * sinx) - (sinz * cosx);
    result.m8 = (cosz * siny * cosx) + (sinz * sinx);

    result.m1 = sinz * cosy;
    result.m5 = (sinz * siny * sinx) + (cosz * cosx);
    result.m9 = (sinz * siny * cosx) - (cosz * sinx);

    result.m2 = -siny;
    result.m6 = cosy * sinx;
    result.m10= cosy * cosx;

    return result;
}

// Returns x-rotation matrix (angle in radians)
MAIMATH_DEF mat4 mat4RotateX(float angle)
{
    mat4 result = mat4Identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m5 = cosres;
    result.m6 = -sinres;
    result.m9 = sinres;
    result.m10 = cosres;

    return result;
}

// Returns y-rotation matrix (angle in radians)
MAIMATH_DEF mat4 mat4RotateY(float angle)
{
    mat4 result = mat4Identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m0 = cosres;
    result.m2 = sinres;
    result.m8 = -sinres;
    result.m10 = cosres;

    return result;
}

// Returns z-rotation matrix (angle in radians)
MAIMATH_DEF mat4 mat4RotateZ(float angle)
{
    mat4 result = mat4Identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m0 = cosres;
    result.m1 = -sinres;
    result.m4 = sinres;
    result.m5 = cosres;

    return result;
}

// Returns scaling matrix
MAIMATH_DEF mat4 mat4Scale(float x, float y, float z)
{
    mat4 result = { x, 0.0f, 0.0f, 0.0f,
                      0.0f, y, 0.0f, 0.0f,
                      0.0f, 0.0f, z, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f };

    return result;
}

// Returns two matrix multiplication
// NOTE: When multiplying matrices... the order matters!
MAIMATH_DEF mat4 mat4Multiply(mat4 left, mat4 right)
{
    mat4 result = { 0 };

    result.m0 = left.m0*right.m0 + left.m1*right.m4 + left.m2*right.m8 + left.m3*right.m12;
    result.m1 = left.m0*right.m1 + left.m1*right.m5 + left.m2*right.m9 + left.m3*right.m13;
    result.m2 = left.m0*right.m2 + left.m1*right.m6 + left.m2*right.m10 + left.m3*right.m14;
    result.m3 = left.m0*right.m3 + left.m1*right.m7 + left.m2*right.m11 + left.m3*right.m15;
    result.m4 = left.m4*right.m0 + left.m5*right.m4 + left.m6*right.m8 + left.m7*right.m12;
    result.m5 = left.m4*right.m1 + left.m5*right.m5 + left.m6*right.m9 + left.m7*right.m13;
    result.m6 = left.m4*right.m2 + left.m5*right.m6 + left.m6*right.m10 + left.m7*right.m14;
    result.m7 = left.m4*right.m3 + left.m5*right.m7 + left.m6*right.m11 + left.m7*right.m15;
    result.m8 = left.m8*right.m0 + left.m9*right.m4 + left.m10*right.m8 + left.m11*right.m12;
    result.m9 = left.m8*right.m1 + left.m9*right.m5 + left.m10*right.m9 + left.m11*right.m13;
    result.m10 = left.m8*right.m2 + left.m9*right.m6 + left.m10*right.m10 + left.m11*right.m14;
    result.m11 = left.m8*right.m3 + left.m9*right.m7 + left.m10*right.m11 + left.m11*right.m15;
    result.m12 = left.m12*right.m0 + left.m13*right.m4 + left.m14*right.m8 + left.m15*right.m12;
    result.m13 = left.m12*right.m1 + left.m13*right.m5 + left.m14*right.m9 + left.m15*right.m13;
    result.m14 = left.m12*right.m2 + left.m13*right.m6 + left.m14*right.m10 + left.m15*right.m14;
    result.m15 = left.m12*right.m3 + left.m13*right.m7 + left.m14*right.m11 + left.m15*right.m15;

    return result;
}

// Returns perspective projection matrix
MAIMATH_DEF mat4 mat4Frustum(double left, double right, double bottom, double top, double near, double far)
{
    mat4 result = { 0 };

    float rl = (float)(right - left);
    float tb = (float)(top - bottom);
    float fn = (float)(far - near);

    result.m0 = ((float) near*2.0f)/rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;

    result.m4 = 0.0f;
    result.m5 = ((float) near*2.0f)/tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;

    result.m8 = ((float)right + (float)left)/rl;
    result.m9 = ((float)top + (float)bottom)/tb;
    result.m10 = -((float)far + (float)near)/fn;
    result.m11 = -1.0f;

    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = -((float)far*(float)near*2.0f)/fn;
    result.m15 = 0.0f;

    return result;
}

// Returns perspective projection matrix
// NOTE: Angle should be provided in radians
MAIMATH_DEF mat4 mat4Perspective(double fovy, double aspect, double near, double far)
{
    double top = near*tan(fovy*0.5);
    double right = top*aspect;
    mat4 result = mat4Frustum(-right, right, -top, top, near, far);

    return result;
}

// Returns orthographic projection matrix
MAIMATH_DEF mat4 mat4Ortho(double left, double right, double bottom, double top, double near, double far)
{
    mat4 result = { 0 };

    float rl = (float)(right - left);
    float tb = (float)(top - bottom);
    float fn = (float)(far - near);

    result.m0 = 2.0f/rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;
    result.m4 = 0.0f;
    result.m5 = 2.0f/tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;
    result.m8 = 0.0f;
    result.m9 = 0.0f;
    result.m10 = -2.0f/fn;
    result.m11 = 0.0f;
    result.m12 = -((float)left + (float)right)/rl;
    result.m13 = -((float)top + (float)bottom)/tb;
    result.m14 = -((float)far + (float)near)/fn;
    result.m15 = 1.0f;

    return result;
}

// Returns camera look-at matrix (view matrix)
MAIMATH_DEF mat4 mat4LookAt(vec3 eye, vec3 target, vec3 up)
{
    mat4 result = { 0 };

    vec3 z = vec3Sub(eye, target);
    z = vec3Normalize(z);
    vec3 x = vec3Cross(up, z);
    x = vec3Normalize(x);
    vec3 y = vec3Cross(z, x);
    y = vec3Normalize(y);

    result.m0 = x.x;
    result.m1 = x.y;
    result.m2 = x.z;
    result.m3 = 0.0f;
    result.m4 = y.x;
    result.m5 = y.y;
    result.m6 = y.z;
    result.m7 = 0.0f;
    result.m8 = z.x;
    result.m9 = z.y;
    result.m10 = z.z;
    result.m11 = 0.0f;
    result.m12 = eye.x;
    result.m13 = eye.y;
    result.m14 = eye.z;
    result.m15 = 1.0f;

    result = mat4Invert(result);

    return result;
}

// Returns float array of matrix data
MAIMATH_DEF float16 mat4ToFloatV(mat4 mat)
{
    float16 buffer = { 0 };

    buffer.v[0] = mat.m0;
    buffer.v[1] = mat.m1;
    buffer.v[2] = mat.m2;
    buffer.v[3] = mat.m3;
    buffer.v[4] = mat.m4;
    buffer.v[5] = mat.m5;
    buffer.v[6] = mat.m6;
    buffer.v[7] = mat.m7;
    buffer.v[8] = mat.m8;
    buffer.v[9] = mat.m9;
    buffer.v[10] = mat.m10;
    buffer.v[11] = mat.m11;
    buffer.v[12] = mat.m12;
    buffer.v[13] = mat.m13;
    buffer.v[14] = mat.m14;
    buffer.v[15] = mat.m15;

    return buffer;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - quat math
//----------------------------------------------------------------------------------

// Returns identity quaternion
MAIMATH_DEF quat quatIdentity(void)
{
    quat result = { 0.0f, 0.0f, 0.0f, 1.0f };
    return result;
}

// Computes the length of a quaternion
MAIMATH_DEF float quatLength(quat q)
{
    float result = (float)sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    return result;
}

// Normalize provided quaternion
MAIMATH_DEF quat quatNormalize(quat q)
{
    quat result = { 0 };

    float length, ilength;
    length = quatLength(q);
    if (length == 0.0f) length = 1.0f;
    ilength = 1.0f/length;

    result.x = q.x*ilength;
    result.y = q.y*ilength;
    result.z = q.z*ilength;
    result.w = q.w*ilength;

    return result;
}

// Invert provided quaternion
MAIMATH_DEF quat quatInvert(quat q)
{
    quat result = q;
    float length = quatLength(q);
    float lengthSq = length*length;

    if (lengthSq != 0.0)
    {
        float i = 1.0f/lengthSq;

        result.x *= -i;
        result.y *= -i;
        result.z *= -i;
        result.w *= i;
    }

    return result;
}

// Calculate two quaternion multiplication
MAIMATH_DEF quat quatMultiply(quat q1, quat q2)
{
    quat result = { 0 };

    float qax = q1.x, qay = q1.y, qaz = q1.z, qaw = q1.w;
    float qbx = q2.x, qby = q2.y, qbz = q2.z, qbw = q2.w;

    result.x = qax*qbw + qaw*qbx + qay*qbz - qaz*qby;
    result.y = qay*qbw + qaw*qby + qaz*qbx - qax*qbz;
    result.z = qaz*qbw + qaw*qbz + qax*qby - qay*qbx;
    result.w = qaw*qbw - qax*qbx - qay*qby - qaz*qbz;

    return result;
}

// Calculate linear interpolation between two quaternions
MAIMATH_DEF quat quatLerp(quat q1, quat q2, float amount)
{
    quat result = { 0 };

    result.x = q1.x + amount*(q2.x - q1.x);
    result.y = q1.y + amount*(q2.y - q1.y);
    result.z = q1.z + amount*(q2.z - q1.z);
    result.w = q1.w + amount*(q2.w - q1.w);

    return result;
}

// Calculate slerp-optimized interpolation between two quaternions
MAIMATH_DEF quat quatNlerp(quat q1, quat q2, float amount)
{
    quat result = quatLerp(q1, q2, amount);
    result = quatNormalize(result);

    return result;
}

// Calculates spherical linear interpolation between two quaternions
MAIMATH_DEF quat quatSlerp(quat q1, quat q2, float amount)
{
    quat result = { 0 };

    float cosHalfTheta =  q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;

    if (fabs(cosHalfTheta) >= 1.0f) result = q1;
    else if (cosHalfTheta > 0.95f) result = quatNlerp(q1, q2, amount);
    else
    {
        float halfTheta = (float) acos(cosHalfTheta);
        float sinHalfTheta = (float) sqrt(1.0f - cosHalfTheta*cosHalfTheta);

        if (fabs(sinHalfTheta) < 0.001f)
        {
            result.x = (q1.x*0.5f + q2.x*0.5f);
            result.y = (q1.y*0.5f + q2.y*0.5f);
            result.z = (q1.z*0.5f + q2.z*0.5f);
            result.w = (q1.w*0.5f + q2.w*0.5f);
        }
        else
        {
            float ratioA = sinf((1 - amount)*halfTheta)/sinHalfTheta;
            float ratioB = sinf(amount*halfTheta)/sinHalfTheta;

            result.x = (q1.x*ratioA + q2.x*ratioB);
            result.y = (q1.y*ratioA + q2.y*ratioB);
            result.z = (q1.z*ratioA + q2.z*ratioB);
            result.w = (q1.w*ratioA + q2.w*ratioB);
        }
    }

    return result;
}

// Calculate quaternion based on the rotation from one vector to another
MAIMATH_DEF quat quatFromvec3Tovec3(vec3 from, vec3 to)
{
    quat result = { 0 };

    float cos2Theta = vec3Dot(from, to);
    vec3 cross = vec3Cross(from, to);

    result.x = cross.x;
    result.y = cross.y;
    result.z = cross.y;
    result.w = 1.0f + cos2Theta;     // NOTE: Added QuaternioIdentity()

    // Normalize to essentially nlerp the original and identity to 0.5
    result = quatNormalize(result);

    // Above lines are equivalent to:
    //quat result = quatNlerp(q, quatIdentity(), 0.5f);

    return result;
}

// Returns a quaternion for a given rotation matrix
MAIMATH_DEF quat quatFrommat4(mat4 mat)
{
    quat result = { 0 };

    float trace = mat4Trace(mat);

    if (trace > 0.0f)
    {
        float s = (float)sqrt(trace + 1)*2.0f;
        float invS = 1.0f/s;

        result.w = s*0.25f;
        result.x = (mat.m6 - mat.m9)*invS;
        result.y = (mat.m8 - mat.m2)*invS;
        result.z = (mat.m1 - mat.m4)*invS;
    }
    else
    {
        float m00 = mat.m0, m11 = mat.m5, m22 = mat.m10;

        if (m00 > m11 && m00 > m22)
        {
            float s = (float)sqrt(1.0f + m00 - m11 - m22)*2.0f;
            float invS = 1.0f/s;

            result.w = (mat.m6 - mat.m9)*invS;
            result.x = s*0.25f;
            result.y = (mat.m4 + mat.m1)*invS;
            result.z = (mat.m8 + mat.m2)*invS;
        }
        else if (m11 > m22)
        {
            float s = (float)sqrt(1.0f + m11 - m00 - m22)*2.0f;
            float invS = 1.0f/s;

            result.w = (mat.m8 - mat.m2)*invS;
            result.x = (mat.m4 + mat.m1)*invS;
            result.y = s*0.25f;
            result.z = (mat.m9 + mat.m6)*invS;
        }
        else
        {
            float s = (float)sqrt(1.0f + m22 - m00 - m11)*2.0f;
            float invS = 1.0f/s;

            result.w = (mat.m1 - mat.m4)*invS;
            result.x = (mat.m8 + mat.m2)*invS;
            result.y = (mat.m9 + mat.m6)*invS;
            result.z = s*0.25f;
        }
    }

    return result;
}

// Returns a matrix for a given quaternion
MAIMATH_DEF mat4 quatTomat4(quat q)
{
    mat4 result = { 0 };

    float x = q.x, y = q.y, z = q.z, w = q.w;

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    float length = quatLength(q);
    float lengthSquared = length*length;

    float xx = x*x2/lengthSquared;
    float xy = x*y2/lengthSquared;
    float xz = x*z2/lengthSquared;

    float yy = y*y2/lengthSquared;
    float yz = y*z2/lengthSquared;
    float zz = z*z2/lengthSquared;

    float wx = w*x2/lengthSquared;
    float wy = w*y2/lengthSquared;
    float wz = w*z2/lengthSquared;

    result.m0 = 1.0f - (yy + zz);
    result.m1 = xy - wz;
    result.m2 = xz + wy;
    result.m3 = 0.0f;
    result.m4 = xy + wz;
    result.m5 = 1.0f - (xx + zz);
    result.m6 = yz - wx;
    result.m7 = 0.0f;
    result.m8 = xz - wy;
    result.m9 = yz + wx;
    result.m10 = 1.0f - (xx + yy);
    result.m11 = 0.0f;
    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

// Returns rotation quaternion for an angle and axis
// NOTE: angle must be provided in radians
MAIMATH_DEF quat quatFromAxisAngle(vec3 axis, float angle)
{
    quat result = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (vec3Length(axis) != 0.0f)

    angle *= 0.5f;

    axis = vec3Normalize(axis);

    float sinres = sinf(angle);
    float cosres = cosf(angle);

    result.x = axis.x*sinres;
    result.y = axis.y*sinres;
    result.z = axis.z*sinres;
    result.w = cosres;

    result = quatNormalize(result);

    return result;
}

// Returns the rotation angle and axis for a given quaternion
MAIMATH_DEF void quatToAxisAngle(quat q, vec3 *outAxis, float *outAngle)
{
    if (fabs(q.w) > 1.0f) q = quatNormalize(q);

    vec3 resAxis = { 0.0f, 0.0f, 0.0f };
    float resAngle = 0.0f;

    resAngle = 2.0f*(float)acos(q.w);
    float den = (float)sqrt(1.0f - q.w*q.w);

    if (den > 0.0001f)
    {
        resAxis.x = q.x/den;
        resAxis.y = q.y/den;
        resAxis.z = q.z/den;
    }
    else
    {
        // This occurs when the angle is zero.
        // Not a problem: just set an arbitrary normalized axis.
        resAxis.x = 1.0f;
    }

    *outAxis = resAxis;
    *outAngle = resAngle;
}

// Returns he quaternion equivalent to Euler angles
MAIMATH_DEF quat quatFromEuler(float roll, float pitch, float yaw)
{
    quat q = { 0 };

    float x0 = cosf(roll*0.5f);
    float x1 = sinf(roll*0.5f);
    float y0 = cosf(pitch*0.5f);
    float y1 = sinf(pitch*0.5f);
    float z0 = cosf(yaw*0.5f);
    float z1 = sinf(yaw*0.5f);

    q.x = x1*y0*z0 - x0*y1*z1;
    q.y = x0*y1*z0 + x1*y0*z1;
    q.z = x0*y0*z1 - x1*y1*z0;
    q.w = x0*y0*z0 + x1*y1*z1;

    return q;
}

// Return the Euler angles equivalent to quaternion (roll, pitch, yaw)
// NOTE: Angles are returned in a vec3 struct in degrees
MAIMATH_DEF vec3 quatToEuler(quat q)
{
    vec3 result = { 0 };

    // roll (x-axis rotation)
    float x0 = 2.0f*(q.w*q.x + q.y*q.z);
    float x1 = 1.0f - 2.0f*(q.x*q.x + q.y*q.y);
    result.x = atan2f(x0, x1)*RAD2DEG;

    // pitch (y-axis rotation)
    float y0 = 2.0f*(q.w*q.y - q.z*q.x);
    y0 = y0 > 1.0f ? 1.0f : y0;
    y0 = y0 < -1.0f ? -1.0f : y0;
    result.y = asinf(y0)*RAD2DEG;

    // yaw (z-axis rotation)
    float z0 = 2.0f*(q.w*q.z + q.x*q.y);
    float z1 = 1.0f - 2.0f*(q.y*q.y + q.z*q.z);
    result.z = atan2f(z0, z1)*RAD2DEG;

    return result;
}

// Transform a quaternion given a transformation matrix
MAIMATH_DEF quat quatTransform(quat q, mat4 mat)
{
    quat result = { 0 };

    result.x = mat.m0*q.x + mat.m4*q.y + mat.m8*q.z + mat.m12*q.w;
    result.y = mat.m1*q.x + mat.m5*q.y + mat.m9*q.z + mat.m13*q.w;
    result.z = mat.m2*q.x + mat.m6*q.y + mat.m10*q.z + mat.m14*q.w;
    result.w = mat.m3*q.x + mat.m7*q.y + mat.m11*q.z + mat.m15*q.w;

    return result;
}

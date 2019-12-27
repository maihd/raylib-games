#pragma once

#include "./MaiDef.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

MAILIB_INLINE float EaseLinear(float start, float end, float time, float duration)
{
    return (end - start) * time / duration + start;
}

MAILIB_INLINE float EaseQuadIn(float start, float end, float time, float duration)
{
    return (end - start) * (time /= duration) * time + start;
}

MAILIB_INLINE float EaseQuadOut(float start, float end, float time, float duration)
{
    return (start - end) * (time /= duration) * (time - 2.0f) + start;
}

MAILIB_INLINE float EaseQuadInOut(float start, float end, float time, float duration)
{
    return (time /= duration * 0.5f) < 1.0f
        ? ((end - start) * 0.5f * time * time + start)
        : ((start - end) * 0.5f * ((time - 1.0f) * (time - 3.0f) - 1.0f) + start);
}

MAILIB_INLINE float EaseCubicIn(float start, float end, float time, float duration)
{
    return start + (end - start) * (time /= duration) * time * time;
}

MAILIB_INLINE float EaseCubicOut(float start, float end, float time, float duration)
{
    return start + (end - start) * ((time = time / duration - 1.0f) * time * time + 1.0f);
}

MAILIB_INLINE float EaseCubicInOut(float start, float end, float time, float duration)
{
    return (time /= duration * 0.5f) < 1.0f
	    ? start + (end - start) * 0.5f * time * time * time
	    : start + (end - start) * 0.5f * ((time -= 2.0f) * time * time + 2.0f);
}

MAILIB_INLINE float EaseQuartIn(float start, float end, float time, float duration)
{
    return (end - start) * (time /= duration) * time * time * time + start;
}

MAILIB_INLINE float EaseQuartOut(float start, float end, float time, float duration)
{
    return (start - end) * ((time = time / duration - 1.0f) * time * time * time - 1.0f) + start; 
}

MAILIB_INLINE float EaseQuartInOut(float start, float end, float time, float duration)
{
    return (time /= duration * 0.5f) < 1.0f
	    ? ((end - start) * 0.5f * time * time * time * time + start)
	    : ((start - end) * 0.5f * ((time -= 2.0f) * time * time * time - 2.0f) + start);
}

MAILIB_INLINE float EaseQuintIn(float start, float end, float time, float duration)
{
    return start + (end - start) * (time /= duration) * time * time * time * time;
}

MAILIB_INLINE float EaseQuintOut(float start, float end, float time, float duration)
{
    return start + (end - start) * ((time = time / duration - 1.0f) * time * time * time * time + 1.0f); 
}

MAILIB_INLINE float EaseQuintInOut(float start, float end, float time, float duration)
{
    return (time /= duration * 0.5f) < 1.0f
	    ? start + (end - start) * 0.5f * time * time * time * time * time
	    : start + (end - start) * 0.5f * ((time -= 2.0f) * time * time * time * time + 2.0f);
}

MAILIB_INLINE float EaseSineIn(float start, float end, float time, float duration)
{
    return (start - end) * cosf(time / duration * PI * 0.5f) + end;
}

MAILIB_INLINE float EaseSineOut(float start, float end, float time, float duration)
{
    return (end - start) * sinf(time / duration * PI * 0.5f) + start;
}

MAILIB_INLINE float EaseSineInOut(float start, float end, float time, float duration)
{
    return (start - end) * 0.5f * (cosf(PI * time / duration) - 1.0f) + start;
}

MAILIB_INLINE float EaseExpoIn(float start, float end, float time, float duration)
{
    return time == 0 ? start : start + (end - start) * powf(2.0f, 10.0f * (time / duration - 1.0f)); 
}

MAILIB_INLINE float EaseExpoOut(float start, float end, float time, float duration)
{
    return time == duration ? end : start + (end - start) * (1.0f - powf(2.0f, -10.0f * time / duration));
}

MAILIB_INLINE float EaseExpoInOut(float start, float end, float time, float duration)
{
    if (time == 0)
    {
	    return start;
    }
    else if (time == duration)
    {
	    return end;
    }
    else if (time < duration * 0.5f)
    {
        return 0.5f * EaseExpoIn(start, end, time * 2.0f, duration);
    }
    else
    {
        return EaseExpoOut(start + (end - start) * 0.5f, end, (time - 0.5f) * 2.0f, duration);
    }
}

MAILIB_INLINE float EaseCircleIn(float start, float end, float time, float duration)
{
    return start + (start - end) * (sqrtf(1.0f - (time /= duration) * time) - 1.0f);
}

MAILIB_INLINE float EaseCircleOut(float start, float end, float time, float duration)
{
    return start + (end - start) * (sqrtf(1 - (time = time / duration - 1.0f) * time));
}

MAILIB_INLINE float EaseCircleInOut(float start, float end, float time, float duration)
{
    return (time /= duration * 0.5f) < 1.0f
        ? start + (start - end) * 0.5f * (sqrtf(1.0f - time * time) - 1.0f)
        : start + (end - start) * 0.5f * (sqrtf(1.0f - (time -= 2.0f) * time) + 1.0f);
}

MAILIB_INLINE float EaseElasticIn(float start, float end, float time, float duration)
{
    if (time == 0)
    {
	    return start;
    }
    else if ((time /= duration) >= 1.0f)
    {
	    return end;
    }

    const float a = (end - start);
    const float b = (duration * 0.3f);
    const float c = (a < 0) ? (b * 0.25f) : b;
    const float p = powf(2, 10.0f * (time -= 1));
    const float q = sinf((time * duration - c) * (2 * PI) / b);
    return -(a * p * q) + start;
}

MAILIB_INLINE float EaseElasticOut(float start, float end, float time, float duration)
{
    if (time == 0)
    {
	    return start;
    }
    else if ((time /= duration) >= 1.0f)
    {
	    return end;
    }

    const float a = (end - start);
    const float b = (duration * 0.3f);
    const float c = (a < 0) ? (b * 0.25f) : b;
    const float p = powf(2, -10.0f * time);
    const float q = sinf((time * duration - c) * (2.0f * PI) / b);
    return (a * p * q) + end;
}

MAILIB_INLINE float EaseElasticInOut(float start, float end, float time, float duration)
{
    if (time == 0)
    {
	    return start;
    }
    else if ((time /= duration) >= 1.0f)
    {
	    return end;
    }

    const float a = (end - start);
    const float b = (duration * 0.3f * 0.15f);
    const float c = (a < 0) ? (b * 0.25f) : b;
    if (time < 1.0f)
    {
	    const float p = powf(2,  10.0f * (time -= 1.0f));
	    const float q = sinf((time * duration - c) * (2.0f * PI) / b);
	    return -0.5f * (a * p * q) + start;
    }
    else
    {
	    const float p = powf(2, -10.0f * (time -= 1.0f));
	    const float q = sinf((time * duration - c) * (2.0f * PI) / b);
	    return  0.5f * (a * p * q) + end;
    }
}

MAILIB_INLINE float EaseBounceOut(float start, float end, float time, float duration)
{
    const float c = (end - start);
    if ((time /= duration) < 1.0f / 2.75f)
    {
	    return c * (7.5625f * time * time) + start;
    }
    else if (time < 2.0f / 2.75f)
    {
	    return c * (7.5625f * (time -= 1.5f / 2.75f) * time + 0.75f) + start;
    }
    else if (time < 2.5f / 2.75f)
    {
	    return c * (7.5625f * (time -= 2.25f / 2.75f) * time + 0.9375f) + start;
    }
    else
    {
	    return c * (7.5625f * (time -= 2.625f / 2.75f) * time + 0.984375f) + start;
    }
}

MAILIB_INLINE float EaseBounceIn(float start, float end, float time, float duration)
{
    return end - EaseBounceOut(start, end, duration - time, duration);
}

MAILIB_INLINE float EaseBounceInOut(float start, float end, float time, float duration)
{
    if (time < duration * 0.5f)
    {
	    return 0.5f * EaseBounceIn(start, end, time * 2.0f, duration);
    }
    else
    {
	    return EaseBounceOut(start + (end - start) * 0.5f, end, (time - 0.5f) * 2.0f, duration);
    }
}

#ifdef __cplusplus
}
#endif

#pragma once

#include <cmath>

#ifdef OP4_PLATFORM_WINDOWS
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE
#endif

namespace opfor::math
{

inline constexpr float PI = 3.1415926535897932f;
inline constexpr float INV_PI = 0.31830988618f;
inline constexpr float HALF_PI = 1.57079632679f;

 /* All single-precision floating point numbers greater than or equal to this have no fractional value. */
constexpr float FLOAT_NON_FRACTIONAL = 8388608.f;

float truncf(float x);

float TruncToFloat(float F);

template <class T> [[nodiscard]] T Clamp(const T X, const T Min, const T Max)
{
    return X < Min ? Min : X < Max ? X : Max;
}

float Fmod(float X, float Y);

float ClampAxis(float Angle);

float NormalizeAxis(float Angle);

template <class T> constexpr T Abs(T A)
{
    return (A >= (T)0) ? A : -A;
}

/**
 * Computes the ASin of a scalar value.
 *
 * @param Value  input angle
 * @return ASin of Value
 */
float FastAsin(float Value);

float Atan2(float Y, float X);

void SinCos(float *ScalarSin, float *ScalarCos, float Value);

/**
 * @brief Convert an angle in degrees to radians
 */
template <typename T>
[[nodiscard]] T Radians(T deg)
{
    return deg * (PI * 2.f) / 360.f;
}

/**
 * @brief Convert an angle in radians to degrees
 */
template <typename T>
[[nodiscard]] T Degrees(T rad)
{
    return rad * (180.f / PI);
}

template <typename T>[[nodiscard]] T Tan(T x)
{
    return std::tan(x);
}

} // namespace opfor

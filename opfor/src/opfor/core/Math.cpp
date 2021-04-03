#include <cmath>
#include "Math.hpp"

namespace opfor::math
{

float truncf(float x)
{
    return x < 0.0f ? ceilf(x) : floorf(x);
}

float TruncToFloat(float F)
{
    return truncf(F);
}

template <class T> T Clamp(const T X, const T Min, const T Max)
{
    return X < Min ? Min : X < Max ? X : Max;
}

float Fmod(float X, float Y)
{
    const float AbsY = fabsf(Y);
    if (AbsY <= 1.e-8f)
    {
        return 0.f;
    }
    const float Div = (X / Y);
    // All floats where abs(f) >= 2^23 (8388608) are whole numbers so do not need truncation, and avoid overflow
    // in TruncToFloat as they get even larger.
    const float Quotient = fabsf(Div) < FLOAT_NON_FRACTIONAL ? TruncToFloat(Div) : Div;
    float IntPortion = Y * Quotient;

    // Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the
    // expected range. For example Fmod(55.8, 9.3) would result in a very small negative value!
    if (fabsf(IntPortion) > fabsf(X))
    {
        IntPortion = X;
    }

    const float Result = X - IntPortion;
    // Clamp to [-AbsY, AbsY] because of possible failures for very large numbers (>1e10) due to precision loss.
    // We could instead fall back to stock fmodf() for large values, however this would diverge from the SIMD
    // VectorMod() which has no similar fallback with reasonable performance.
    return Clamp(Result, -AbsY, AbsY);
}

float ClampAxis(float Angle)
{
    // returns Angle in the range (-360,360)
    Angle = Fmod(Angle, 360.f);

    if (Angle < 0.f)
    {
        // shift to [0,360) range
        Angle += 360.f;
    }

    return Angle;
}

float NormalizeAxis(float Angle)
{
    // returns Angle in the range [0,360)
    Angle = ClampAxis(Angle);

    if (Angle > 180.f)
    {
        // shift to (-180,180]
        Angle -= 360.f;
    }

    return Angle;
}

template <class T> constexpr T Abs(const T A)
{
    return (A >= (T)0) ? A : -A;
}

#define FASTASIN_HALF_PI (1.5707963050f)
/**
 * Computes the ASin of a scalar value.
 *
 * @param Value  input angle
 * @return ASin of Value
 */
float FastAsin(float Value)
{
    // Clamp input to [-1,1].
    bool nonnegative = (Value >= 0.0f);
    float x = Abs(Value);
    float omx = 1.0f - x;
    if (omx < 0.0f)
    {
        omx = 0.0f;
    }
    float root = std::sqrt(omx);
    // 7-degree minimax approximation
    float result =
        ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x +
          0.0889789874f) *
             x -
         0.2145988016f) *
            x +
        FASTASIN_HALF_PI;
    result *= root; // acos(|x|)
    // acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
    return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
}
#undef FASTASIN_HALF_PI

float Atan2(float Y, float X)
{
    // return atan2f(Y,X);
    // atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
    // We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to
    // the C library function. On PC this has been measured to be 2x faster than the std C version.

    const float absX = Abs(X);
    const float absY = Abs(Y);
    const bool yAbsBigger = (absY > absX);
    float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
    float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

    if (t0 == 0.f)
        return 0.f;

    float t3 = t1 / t0;
    float t4 = t3 * t3;

    const float c[7] = {+7.2128853633444123e-03f,
                               -3.5059680836411644e-02f,
                               +8.1675882859940430e-02f,
                               -1.3374657325451267e-01f,
                               +1.9856563505717162e-01f,
                               -3.3324998579202170e-01f,
                               +1.0f};

    t0 = c[0];
    t0 = t0 * t4 + c[1];
    t0 = t0 * t4 + c[2];
    t0 = t0 * t4 + c[3];
    t0 = t0 * t4 + c[4];
    t0 = t0 * t4 + c[5];
    t0 = t0 * t4 + c[6];
    t3 = t0 * t3;

    t3 = yAbsBigger ? (0.5f * PI) - t3 : t3;
    t3 = (X < 0.0f) ? PI - t3 : t3;
    t3 = (Y < 0.0f) ? -t3 : t3;

    return t3;
}

void SinCos( float* ScalarSin, float* ScalarCos, float  Value )
{
    // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
    float quotient = (INV_PI*0.5f)*Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - (2.0f*PI)*quotient;

    // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
    float sign;
    if (y > HALF_PI)
    {
        y = PI - y;
        sign = -1.0f;
    }
    else if (y < -HALF_PI)
    {
        y = -PI - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    // 11-degree minimax approximation
    *ScalarSin = ( ( ( ( (-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f ) * y2 + 0.0083333310f ) * y2 - 0.16666667f ) * y2 + 1.0f ) * y;

    // 10-degree minimax approximation
    float p = ( ( ( ( -2.6051615e-07f * y2 + 2.4760495e-05f ) * y2 - 0.0013888378f ) * y2 + 0.041666638f ) * y2 - 0.5f ) * y2 + 1.0f;
    *ScalarCos = sign*p;
}

} // namespace opfor

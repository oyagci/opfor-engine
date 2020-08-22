//
// Created by Marc on 07/02/2020.
//

#pragma once

#include <math.h>

#define PI 3.14159265358979323846264338327950288419716939937510582

#define TO_RADIANS(x) (float) (x * PI / 180.0f)
#define TO_DEGREES(x) (float) (x * 180.0f / PI)

#define SQRT(x) (float) (sqrt(x))
#define POW(x) (float) (pow(x))

#define COS(x) (float) (cos(x))
#define SIN(x) (float) (sin(x))
#define TAN(x) (float) (tan(x))

#define ACOS(x) (float) (acos(x))
#define ASIN(x) (float) (asin(x))
#define ATAN(x) (float) (atan(x))
#define ATAN2(x) (float) (atan2(x))

#define LERP(a, b, t) (float) (a + (b - a) * t)

#define MAX(a, b) (float) (a > b ? a : b)
#define MIN(a, b) (float) (a < b ? a : b)

namespace lazy
{
	namespace maths
	{
        float   random();
    }
}
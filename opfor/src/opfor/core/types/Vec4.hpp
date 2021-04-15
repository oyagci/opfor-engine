#pragma once

#include <opfor/core/base.hpp>
#include <opfor/core/types/fwd.hpp>
#include <generated/Vec4.rfk.h>

namespace opfor OP4NAMESPACE()
{

class OP4CLASS() Vec4
{
public:
    Vec4()
    {
    }

    Vec4(float v) : x(v), y(v), z(v), w(v)
    {
    }

    Vec4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    {
    }

    Vec4(Vec3 const &a);

    Vec4 operator*(Vec4 const &rhs) const
    {
        return {x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w};
    }

    Vec4 operator*(float rhs) const
    {
        return {x * rhs, y * rhs, z * rhs, w * rhs};
    }

    Vec4 &operator*=(Vec4 const &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;

        return *this;
    }

    Vec4 &operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;

        return *this;
    }

    Vec4 operator+(Vec4 const &rhs) const
    {
        return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
    }

    Vec4 operator+(float rhs) const
    {
        return {x + rhs, y + rhs, z + rhs, w + rhs};
    }

    float &operator[](size_t idx)
    {
        switch (idx)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        default:
            OP4_ABORT();
        }
    }

    float const &operator[](size_t idx) const
    {
        switch (idx)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        default:
            OP4_ABORT();
        }
    }

    Vec4 operator-(Vec4 const &rhs) const
    {
        return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
    }

    bool operator==(Vec4 const &rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool operator!=(Vec4 const &rhs) const
    {
        return !(*this == rhs);
    }

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;

    Vec4_GENERATED
};

} // namespace opfor

File_GENERATED

#pragma once

#include <opfor/core/base.hpp>
#include <Refureku/TypeInfo/Archetypes/Class.h>
#include <Refureku/TypeInfo/Archetypes/GetArchetype.h>
#include <generated/Vec3.rfk.h>

namespace opfor OP4NAMESPACE()
{

template <typename T>
class AVec3
{
  public:
    AVec3() = default;

    AVec3(T const X, T const Y, T const Z) : x(X), y(Y), z(Z)
    {
    }

    AVec3(AVec3<T> const &v) : x(v.x), y(v.y), z(v.z)
    {
    }

    AVec3(T scalar) : x(scalar), y(scalar), z(scalar)
    {
    }

    AVec3<T> operator+(AVec3<T> const &rhs) const
    {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }

    AVec3<T> operator*(AVec3<T> const &rhs) const
    {
        return {x * rhs.x, y * rhs.y, z * rhs.z};
    }

    AVec3<T> operator*(T v) const
    {
        return {x * v, y * v, z * v};
    }

    AVec3<T> operator/(T v) const
    {
        return {x / v, y / v, z / v};
    }

    AVec3<T> operator-() const
    {
        return {-x, -y, -z};
    }

    AVec3<T> &operator+=(AVec3<T> const &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    T &operator[](size_t idx)
    {
        switch (idx)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            OP4_ABORT();
        }
    }

    T const &operator[](size_t idx) const
    {
        switch (idx)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            OP4_ABORT();
        }
    }

    AVec3<T> operator-(AVec3<T> const &rhs) const
    {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    AVec3<T> &operator*=(T v)
    {
        x *= v;
        y *= v;
        z *= v;

        return *this;
    }

    bool operator==(AVec3<T> const &rhs) const
    {
        return x == rhs.x || y == rhs.y || z == rhs.z;
    }

    bool operator!=(AVec3<T> const &rhs) const
    {
        return !(*this == rhs);
    }

    [[nodiscard]] AVec3<T> Scale(T desiredLength) const
    {
        return *this * desiredLength / Magnitude();
    }

    [[nodiscard]] AVec3<T> Normalized() const
    {
        const T len = Magnitude();
        return AVec3<T>(x / len, y / len, z / len);
    }

    [[nodiscard]] T Magnitude() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    static T Magnitude(AVec3<T> const &v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    [[nodiscard]] static AVec3<T> Normalize(AVec3<T> const &a)
    {
        return a.Normalized();
    }

    [[nodiscard]] static AVec3<T> Cross(AVec3<T> const &a, AVec3<T> const &b)
    {
        return AVec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    [[nodiscard]] static T Dot(AVec3<T> const &a , AVec3<T> const &b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    [[nodiscard]] static AVec3<T> Forward()
    {
        return AVec3(0.f, 0.f, -1.f);
    }

    [[nodiscard]] static AVec3<T> Up()
    {
        return AVec3(0.f, 1.f, 0.f);
    }

    [[nodiscard]] static AVec3<T> Right()
    {
        return AVec3(1.f, 0.f, 0.f);
    }

    T x = T(0);
    T y = T(0);
    T z = T(0);

    friend AVec3<T> operator*(T const &lhs, AVec3<T> const &rhs)
    {
        return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
    }

    friend AVec3<T> operator+(T const &lhs, AVec3<T> const &rhs)
    {
        return {lhs + rhs.x, lhs + rhs.y, lhs + rhs.z};
    }
};

class OP4CLASS() Vec3 : public AVec3<float>
{
public:
    using AVec3<float>::AVec3;

    Vec3(AVec3<float> const &v) : AVec3<float>(v)
    {
    }

    Vec3_GENERATED
};

class OP4CLASS() UVec3 : public AVec3<unsigned int>
{
public:
    using AVec3<unsigned int>::AVec3;

    UVec3(AVec3<unsigned int> const &v) : AVec3<unsigned int>(v)
    {
    }

    UVec3_GENERATED
};

} // namespace opfor

File_GENERATED

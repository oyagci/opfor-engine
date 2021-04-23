#pragma once

#include <opfor/core/Math.hpp>
#include <opfor/core/types/Rotator.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <generated/Quat.rfk.h>

namespace opfor OP4NAMESPACE()
{

class Mat4;

class OP4CLASS() Quat
{
  public:
    Quat() = default;

    Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z)
    {
    }

    Quat(Quat const &other) : w(other.w), x(other.x), y(other.y), z(other.z)
    {
    }

    static Quat Identity()
    {
        return Quat(1.f, 0.f, 0.f, 0.f);
    }

    explicit Quat(Vec3 const &eulerAngles)
    {
        Vec3 c(std::cos(eulerAngles.x * 0.5f), std::cos(eulerAngles.y * 0.5f), std::cos(eulerAngles.z * 0.5f));
        Vec3 s(std::sin(eulerAngles.x * 0.5f), std::sin(eulerAngles.y * 0.5f), std::sin(eulerAngles.z * 0.5f));

        this->w = c.x * c.y * c.z + s.x * s.y * s.z;
        this->x = s.x * c.y * c.z - c.x * s.y * s.z;
        this->y = c.x * s.y * c.z + s.x * c.y * s.z;
        this->z = c.x * c.y * s.z - s.x * s.y * c.z;
    }

    Quat &operator=(Quat const &other) = default;

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

    Quat &operator*=(Quat const &rhs)
    {
        Quat const p(*this);
        Quat const q(rhs);

        this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
        this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
        this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
        this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
        return *this;
    }

    Quat operator*(Quat const &rhs) const
    {
        return Quat(*this) *= rhs;
    }

    void Rotate(Vec3 const &v, float angle)
    {
        Vec3 tmp = v;

        // Axis of rotation must be normalised
        const float len = tmp.Magnitude();
        if (std::abs(len - 1.f) > 0.001f)
        {
            tmp = tmp.Normalized();
        }

        float const angleRad(math::Radians(angle));
        float const factor = std::sin(angleRad * 0.5f);

        *this *= Quat(std::cos(angleRad * 0.5f), tmp.x * factor, tmp.y * factor, tmp.z * factor);
    }

    [[nodiscard]] Rotator AsRotator() const;

    [[nodiscard]] Mat4 AsRotationMatrix() const;

    static Quat CreateFromAxisAngle(Vec3 axis, float angle)
    {
        float halfAngle = angle * .5f;
        float s = std::sin(halfAngle);
        Quat q;
        q.x = axis.x * s;
        q.y = axis.y * s;
        q.z = axis.z * s;
        q.w = std::cos(halfAngle);
        return q;
    }

    [[nodiscard]] Vec3 EulerAngles() const
    {
        return Vec3(Pitch(), Yaw(), Roll());
    }

    [[nodiscard]] float Roll() const
    {
        return std::atan2(2.f * (x * y + w * z), w * w + x * x - y * y - z * z);
    }

    [[nodiscard]] float Pitch() const
    {
        float const b = 2.f * (y * z + w * x);
        float const a = w * w - x * x - y * y + z * z;

        if (math::Abs(a) < std::numeric_limits<float>::epsilon() &&
            math::Abs(b) < std::numeric_limits<float>::epsilon())
        {
            return 2.f * math::Atan2(x, w);
        }

        return std::atan2(b, a);
    }

    [[nodiscard]] float Yaw() const
    {
        return math::FastAsin(math::Clamp(-2.f * (x * z - w * y), -1.f, 1.f));
    }

    float w = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Quat_GENERATED
};

} // namespace opfor

File_GENERATED


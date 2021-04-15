#pragma once
#include <opfor/core/types/fwd.hpp>

namespace opfor
{

struct Rotator
{
    float Pitch = 0.0f;
    float Yaw = 0.0f;
    float Roll = 0.0f;

    Rotator() = default;
    Rotator(const Quat &q);
    Rotator(const Vec3 &v);

    Rotator &operator=(const Rotator &other);

	Quat AsQuaternion() const;
};

}

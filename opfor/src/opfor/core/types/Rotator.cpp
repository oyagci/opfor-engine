#include "Rotator.hpp"

#include <opfor/core/types/Quat.hpp>
#include <opfor/core/Math.hpp>
#include <opfor/core/types/Vec3.hpp>

namespace opfor
{

Rotator::Rotator(const Quat &q)
{
    *this = q.AsRotator();
}

Rotator::Rotator(const Vec3 &v)
{
    Pitch = v.x;
    Yaw = v.y;
    Roll = v.z;
}

Rotator &Rotator::operator=(const Rotator &other) = default;

Quat Rotator::AsQuaternion() const
{
    const float DEG_TO_RAD = math::PI / (180.f);
    const float RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2.f;
    float SP, SY, SR;
    float CP, CY, CR;

    const float PitchNoWinding = math::Fmod(Pitch, 360.0f);
    const float YawNoWinding = math::Fmod(Yaw, 360.0f);
    const float RollNoWinding = math::Fmod(Roll, 360.0f);

    math::SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
    math::SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
    math::SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);

    Quat RotationQuat;
    RotationQuat.x = CR * SP * SY - SR * CP * CY;
    RotationQuat.y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.z = CR * CP * SY - SR * SP * CY;
    RotationQuat.w = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

}

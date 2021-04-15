#include "Quat.hpp"
#include <opfor/core/types/Mat4.hpp>

namespace opfor
{

Rotator Quat::AsRotator() const
{
    auto const singularityTest = z * x - w * y;
    auto const yawY = 2.f * (w * z + x * y);
    auto const yawX = (1.f - 2.f * (y * y + z * z));

    auto const singularityThreshold = 0.4999995f;
    auto const radToDeg = (180.f) / math::PI;

    Rotator rotatorFromQuat;

    if (singularityTest < -singularityThreshold)
    {
        rotatorFromQuat.Pitch = -90.f;
        rotatorFromQuat.Yaw = math::Atan2(yawY, yawX) * radToDeg;
        rotatorFromQuat.Roll =
            math::NormalizeAxis(-rotatorFromQuat.Yaw - (2.f * math::Atan2(x, w) * radToDeg));
    }
    else if (singularityTest > singularityThreshold)
    {
        rotatorFromQuat.Pitch = 90.f;
        rotatorFromQuat.Yaw = math::Atan2(yawY, yawX) * radToDeg;
        rotatorFromQuat.Roll =
            math::NormalizeAxis(rotatorFromQuat.Yaw - (2.f * math::Atan2(x, w) * radToDeg));
    }
    else
    {
        rotatorFromQuat.Pitch = math::FastAsin(2.f * (singularityTest)) * radToDeg;
        rotatorFromQuat.Yaw = math::Atan2(yawY, yawX) * radToDeg;
        rotatorFromQuat.Roll = math::Atan2(-2.f * (w * x + y * z), (1.f - 2.f * (x * x + y * y))) * radToDeg;
    }

    return rotatorFromQuat;
}

Mat4 Quat::AsRotationMatrix() const
{
    Mat4 result(1.f);
    float const qxx(x * x);
    float const qyy(y * y);
    float const qzz(z * z);
    float const qxz(x * z);
    float const qxy(x * y);
    float const qyz(y * z);
    float const qwx(w * x);
    float const qwy(w * y);
    float const qwz(w * z);

    result[0][0] = 1.f - 2.f * (qyy + qzz);
    result[0][1] = 2.f * (qxy + qwz);
    result[0][2] = 2.f * (qxz - qwy);

    result[1][0] = 2.f * (qxy - qwz);
    result[1][1] = 1.f - 2.f * (qxx + qzz);
    result[1][2] = 2.f * (qyz + qwx);

    result[2][0] = 2.f * (qxz + qwy);
    result[2][1] = 2.f * (qyz - qwx);
    result[2][2] = 1.f - 2.f * (qxx + qyy);

    return result;
}

} // namespace opfor

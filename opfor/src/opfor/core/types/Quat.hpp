#pragma once

#include <glm/gtx/quaternion.hpp>
#include <opfor/core/Math.hpp>
#include <opfor/core/types/Rotator.hpp>
#include <generated/Quat.rfk.h>

namespace opfor OP4NAMESPACE()
{
    class OP4CLASS() Quat : public glm::quat
    {
      public:
        using glm::quat::quat;

        Quat(glm::quat const &q) : glm::quat(q)
        {
        }

        [[nodiscard]] Rotator AsRotator() const
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

        Quat_GENERATED
    };
} // namespace opfor

File_GENERATED


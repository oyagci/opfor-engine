#pragma once

#include <glm/vec3.hpp>
#include <generated/Vec3.rfk.h>

namespace opfor OP4NAMESPACE()
{
    class OP4CLASS() Vec3 : public glm::vec3
    {
      public:
        using glm::vec3::vec3;

        Vec3(glm::vec3 const &v) : glm::vec3(v)
        {
        }

        Vec3_GENERATED
    };
} // namespace opfor

File_GENERATED

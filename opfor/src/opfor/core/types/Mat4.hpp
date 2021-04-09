#pragma once

#include <glm/mat4x4.hpp>
#include <generated/Mat4.rfk.h>

namespace opfor OP4NAMESPACE()
{
    class OP4CLASS() Mat4 : public glm::mat4
    {
      public:
        using glm::mat4::mat4;

        Mat4(glm::mat4 const &v) : glm::mat4(v)
        {
        }

        Mat4_GENERATED
    };
} // namespace opfor

File_GENERATED

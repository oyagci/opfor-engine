#pragma once

#include <glm/vec4.hpp>
#include <generated/Vec4.rfk.h>

namespace opfor OP4NAMESPACE()
{
    class OP4CLASS() Vec4 : public glm::vec4
    {
      public:
        using glm::vec4::vec4;

        Vec4(glm::vec4 const &v) : glm::vec4(v)
        {
        }

        Vec4_GENERATED
    };
} // namespace opfor

File_GENERATED

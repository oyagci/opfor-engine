#pragma once

#include <glm/vec2.hpp>
#include <generated/Vec2.rfk.h>

namespace opfor OP4NAMESPACE()
{
    class OP4CLASS() Vec2 : public glm::vec2
    {
      public:
        using glm::vec2::vec2;

        Vec2(glm::vec2 const &v) : glm::vec2(v)
        {
        }

        Vec2_GENERATED
    };

    class OP4CLASS() UVec2 : public glm::uvec2
    {
      public:
        using glm::uvec2::uvec2;

        UVec2(glm::uvec2 const &v) : glm::uvec2(v)
        {
        }

        UVec2_GENERATED
    };
} // namespace opfor

File_GENERATED

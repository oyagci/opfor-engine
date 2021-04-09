#pragma once

#include <glm/mat3x3.hpp>
#include <generated/Mat3.rfk.h>

namespace opfor OP4NAMESPACE()
{
    class OP4CLASS() Mat3 : public glm::mat3
    {
      public:
        using glm::mat3::mat3;

        Mat3(glm::mat3 const &v) : glm::mat3(v)
        {
        }

        Mat3_GENERATED
    };
} // namespace opfor

File_GENERATED

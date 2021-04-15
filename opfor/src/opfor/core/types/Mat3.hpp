#pragma once

#include <opfor/core/types/Vec3.hpp>
#include <generated/Mat3.rfk.h>

namespace opfor OP4NAMESPACE()
{

class Mat4;

class OP4CLASS() Mat3
{
public:
    Mat3() = default;

    Mat3(float v);

    Mat3(Mat4 const &m);

    Vec3 &operator[](size_t idx);
    Vec3 const &operator[](size_t idx) const;

private:
    Vec3 _values[3] = {};

    Mat3_GENERATED
};

} // namespace opfor

File_GENERATED

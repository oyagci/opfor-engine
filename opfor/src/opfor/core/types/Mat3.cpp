#include "Mat3.hpp"
#include "Mat4.hpp"
#include <opfor/core/base.hpp>

namespace opfor
{

Mat3::Mat3(Mat4 const &m)
{
    this->operator[](0) = Vec3(m[0].x, m[0].y, m[0].z);
    this->operator[](1) = Vec3(m[1].x, m[1].y, m[1].z);
    this->operator[](2) = Vec3(m[2].x, m[2].y, m[2].z);
}

Vec3 &Mat3::operator[](size_t idx)
{
    OP4_CORE_ASSERT(idx <= sizeof(_values) / sizeof(*_values), "");
    return _values[idx];
}

Vec3 const &Mat3::operator[](size_t idx) const
{
    OP4_CORE_ASSERT(idx <= sizeof(_values) / sizeof(*_values), "");
    return _values[idx];
}

}
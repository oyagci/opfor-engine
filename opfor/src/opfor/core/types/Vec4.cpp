#include "Vec4.hpp"
#include <opfor/core/types/Vec3.hpp>

namespace opfor
{

Vec4::Vec4(Vec3 const &a) : x(a.x), y(a.y), z(a.z), w(0.f)
{
}

}
#pragma once

#include <opfor/core/types/Vec4.hpp>
#include <opfor/core/types/fwd.hpp>
#include <generated/Mat4.rfk.h>

namespace opfor OP4NAMESPACE()
{

class OP4CLASS() Mat4
{
  public:
    Mat4() = default;

    Mat4(float x)
        : _values{
            Vec4(x, 0.f, 0.f, 0.f),
            Vec4(0.f, x, 0.f, 0.f),
            Vec4(0.f, 0.f, x, 0.f),
            Vec4(0.f, 0.f, 0.f, x)}
    {
    }

    Mat4(Vec4 const &v0, Vec4 const &v1, Vec4 const &v2, Vec4 const &v3) : _values{v0, v1, v2, v3}
    {
    }

    Mat4(Mat3 const &m);

    Vec4 &operator[](size_t idx);
    Vec4 const &operator[](size_t idx) const;

    Mat4 operator*(float v) const
    {
        return Mat4(_values[0] * v, _values[1] * v, _values[2] * v, _values[3] * v);
    }

    Mat4 operator*(Mat4 const &other) const
    {
        Vec4 const srcA0 = _values[0];
        Vec4 const srcA1 = _values[1];
        Vec4 const srcA2 = _values[2];
        Vec4 const srcA3 = _values[3];

        Vec4 const srcB0 = other[0];
        Vec4 const srcB1 = other[1];
        Vec4 const srcB2 = other[2];
        Vec4 const srcB3 = other[3];

        Mat4 result;
        result[0] = srcA0 * srcB0[0] + srcA1 * srcB0[1] + srcA2 * srcB0[2] + srcA3 * srcB0[3];
        result[1] = srcA0 * srcB1[0] + srcA1 * srcB1[1] + srcA2 * srcB1[2] + srcA3 * srcB1[3];
        result[2] = srcA0 * srcB2[0] + srcA1 * srcB2[1] + srcA2 * srcB2[2] + srcA3 * srcB2[3];
        result[3] = srcA0 * srcB3[0] + srcA1 * srcB3[1] + srcA2 * srcB3[2] + srcA3 * srcB3[3];

        return result;
    }

    Mat4 &operator*=(float v)
    {
        _values[0] *= v;
        _values[1] *= v;
        _values[2] *= v;
        _values[3] *= v;

        return *this;
    }

    Vec4 operator*(Vec4 const &v) const;

    Mat4 &operator*=(Mat4 const &rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    bool operator==(Mat4 const &rhs) const
    {
        return _values[0] == rhs[0] && _values[1] == rhs[1] && _values[2] == rhs[2] && _values[3] == rhs[3];
    }

    bool operator!=(Mat4 const &rhs) const
    {
        return !(*this == rhs);
    }

    /**
     * @brief Create a translation matrix
     */
    [[nodiscard]] static Mat4 Translate(Vec3 const &translation);

    /**
     * @brief Create a scale matrix
     */
    [[nodiscard]] static Mat4 Scale(Vec3 const &scale);

    /**
     * @brief Create a rotation matrix
     *
     * @param angle The angle in radians
     * @param axis The axis around which we want to do the rotation
     */
    [[nodiscard]] static Mat4 Rotate(float angle, Vec3 const &axis);

    /**
     * @brief Create an identity matrix
     */
     [[nodiscard]] static Mat4 Identity();

    /**
     * @brief Create a perspective projection matrix
     */
    [[nodiscard]] static Mat4 Perspective(float fov, float aspect, float zNear, float zFar);

    /**
     * @brief Create a "Look At" matrix
     */
    [[nodiscard]] static Mat4 LookAt(Vec3 worldPos, Vec3 targetWorldPos, Vec3 up);

    [[nodiscard]] static Mat4 Inverse(Mat4 const &other);

    [[nodiscard]] static bool Decompose(Mat4 const &model, Vec3 &scale, Quat &rotation, Vec3 &translation, Vec3 &skew, Vec4 &perspective);

    [[nodiscard]] static Mat4 Transpose(Mat4 const &m);

    [[nodiscard]] Quat AsQuaternion() const;

  private:
    Vec4 _values[4] = {};

    Mat4_GENERATED
};

} // namespace opfor

File_GENERATED

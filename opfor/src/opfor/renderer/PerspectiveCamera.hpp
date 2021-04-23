#pragma once

#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Mat4.hpp>
#include <opfor/core/types/Quat.hpp>

namespace opfor
{

class PerspectiveCamera
{
  private:
    Mat4 _Projection;
    Mat4 _View;
    Mat4 _ViewProjection;

    Vec3 _Position;

    Quat _Rotation;

    float _FOV = 90.f;
    float _Near = 1.f;
    float _Far = 1000000.f;
    float _Aspect = 16.f / 9.f;
    float _Exposure = 1.f;

    void RecalculateViewMatrix()
    {
        _View = Mat4::LookAt(_Position, _Position + GetForward(), Vec3::Up());
        _ViewProjection = _Projection * _View;
    }

  public:
    PerspectiveCamera()
    {
        _Projection = Mat4::Perspective(math::Radians(_FOV), _Aspect, _Near, _Far);
        _View = Mat4::LookAt(_Position, _Position + GetForward(), Vec3::Up());
        _ViewProjection = _Projection * _View;
    }

    PerspectiveCamera(float aspect, float fov, float nearr, float farr)
        : _Position(0.0f, 0.0f, 0.0f), _FOV(fov), _Near(nearr), _Far(farr), _Aspect(aspect)
    {
        _Projection = Mat4::Perspective(math::Radians(fov), aspect, nearr, farr);
        _View = Mat4::LookAt(_Position, _Position + GetForward(), Vec3::Up());
        _ViewProjection = _Projection * _View;
    }

    auto LookAt(Vec3 const &pos)
    {
        _Rotation = Mat4::Inverse(Mat4::LookAt(_Position, pos, Vec3::Up())).AsQuaternion();
        RecalculateViewMatrix();
    }

    [[nodiscard]] auto GetExposure() const
    {
        return _Exposure;
    }

    [[nodiscard]] auto const &GetPosition() const
    {
        return _Position;
    }

    [[nodiscard]] Vec3 GetDirection() const
    {
        return GetForward();
    }

    [[nodiscard]] auto const &GetProjection() const
    {
        return _Projection;
    }

    [[nodiscard]] auto const &GetViewMatrix() const
    {
        return _View;
    }

    [[nodiscard]] auto const &GetViewProjectionMatrix() const
    {
        return _ViewProjection;
    }

    [[nodiscard]] auto &GetRotation() const
    {
        return _Rotation;
    }

    [[nodiscard]] Vec3 GetForward() const
    {
        const Mat4 rot = _Rotation.AsRotationMatrix();
        const Vec4 v = rot * Vec4(Vec3::Forward().x, Vec3::Forward().y, Vec3::Forward().z, 1.f);

        return Vec3(v.x, v.y, v.z);
    }

    [[nodiscard]] auto GetRight() const
    {
        return Vec3::Cross(GetForward(), Vec3(0.f, 1.f, 0.f));
    }

    [[nodiscard]] auto GetUp() const
    {
        const Vec3 fwd(GetForward());
        const Vec3 up(Vec3::Up());
        const Vec3 right = Vec3::Cross(GetForward(), up);
        return Vec3::Cross(fwd, right);
    }

    auto SetProjection(float aspect, float fov, float nearr, float farr)
    {
        _Projection = Mat4::Perspective(math::Radians(fov), aspect, nearr, farr);
        _ViewProjection = _Projection * _View;
    }

    auto SetAspect(float aspect)
    {
        _Projection = Mat4::Perspective(math::Radians(_FOV), aspect, _Near, _Far);
        _ViewProjection = _Projection * _View;
    }

    auto SetPosition(Vec3 const &position)
    {
        _Position = position;
        RecalculateViewMatrix();
    }

    auto SetRotation(Quat const &rotation)
    {
        _Rotation = rotation;
        RecalculateViewMatrix();
    }
};

} // namespace opfor

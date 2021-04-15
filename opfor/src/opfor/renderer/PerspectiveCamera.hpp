#pragma once

#include "components/TransformComponent.hpp"
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Mat4.hpp>

namespace opfor
{

class PerspectiveCamera
{
  private:
    Mat4 _Projection;
    Mat4 _View;
    Mat4 _ViewProjection;

    Vec3 _Position;
    Vec3 _Direction;

    float _FOV;
    float _Near;
    float _Far;
    float _Aspect;
    float _Exposure;

    float _Yaw;
    float _Pitch;
    float _Roll;

    void RecalculateViewMatrix()
    {
        _Direction.x = cos(math::Radians(_Yaw)) * cos(math::Radians(_Pitch));
        _Direction.y = sin(math::Radians(_Pitch));
        _Direction.z = sin(math::Radians(_Yaw)) * cos(math::Radians(_Pitch));

        _View = Mat4::LookAt(_Position, _Position + _Direction, Vec3(0.0f, 1.0f, 0.0f));
        _ViewProjection = _Projection * _View;
    }

  public:
    PerspectiveCamera()
    {
        _Position = {0.0f, 0.0f, 0.0f};
        _Direction = {0.0f, 0.0f, 0.0f};

        _FOV = 90.0f;
        _Aspect = 16.0f / 9.0f;
        _Near = 1.0f;
        _Far = 1000000.0f;
        _Exposure = 1.0f;

        _Yaw = 0.0f;
        _Pitch = 0.0f;
        _Roll = 0.0f;

        _Direction.x = cos(math::Radians(_Yaw)) * cos(math::Radians(_Pitch));
        _Direction.y = sin(math::Radians(_Pitch));
        _Direction.z = sin(math::Radians(_Yaw)) * cos(math::Radians(_Pitch));

        _Projection = Mat4::Perspective(math::Radians(_FOV), _Aspect, _Near, _Far);
        _View = Mat4::LookAt(_Position, _Position + _Direction, Vec3(0.0f, 1.0f, 0.0f));
        _ViewProjection = _Projection * _View;
    }

    PerspectiveCamera(float aspect, float fov, float nearr, float farr)
        : _Position(0.0f, 0.0f, 0.0f), _FOV(fov), _Near(nearr), _Far(farr), _Aspect(aspect), _Exposure(1.0f),
          _Yaw(0.0f), _Pitch(0.0f), _Roll(0.0f)
    {
        _Direction.x = cos(math::Radians(_Yaw)) * cos(math::Radians(_Pitch));
        _Direction.y = sin(math::Radians(_Pitch));
        _Direction.z = sin(math::Radians(_Yaw)) * cos(math::Radians(_Pitch));

        _Projection = Mat4::Perspective(math::Radians(fov), aspect, nearr, farr);
        _View = Mat4::LookAt(_Position, _Position + _Direction, Vec3(0.0f, 1.0f, 0.0f));
        _ViewProjection = _Projection * _View;
    }

    [[nodiscard]] auto GetExposure() const
    {
        return _Exposure;
    }

    [[nodiscard]] auto const &GetPosition() const
    {
        return _Position;
    }

    [[nodiscard]] auto const &GetDirection() const
    {
        return _Direction;
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

    [[nodiscard]] auto GetYaw() const
    {
        return _Yaw;
    }

    [[nodiscard]] auto GetPitch() const
    {
        return _Pitch;
    }

    [[nodiscard]] auto GetRoll() const
    {
        return _Roll;
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

    auto SetPosition(Vec3 position)
    {
        _Position = position;
        RecalculateViewMatrix();
    }

    auto SetYaw(float yaw)
    {
        _Yaw = yaw;
        RecalculateViewMatrix();
    }

    auto SetPitch(float pitch)
    {
        _Pitch = pitch;
        RecalculateViewMatrix();
    }

    auto SetRoll(float roll)
    {
        _Roll = roll;
        RecalculateViewMatrix();
    }
};

} // namespace opfor

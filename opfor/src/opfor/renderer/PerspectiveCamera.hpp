#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "opfor/core/Input.hpp"

namespace opfor
{

class PerspectiveCamera
{
  private:
    glm::mat4 _Projection;
    glm::mat4 _View;
    glm::mat4 _ViewProjection;

    glm::vec3 _Position;
    glm::vec3 _Direction;

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
        _Direction.x = cos(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));
        _Direction.y = sin(glm::radians(_Pitch));
        _Direction.z = sin(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));

        _View = glm::lookAt(_Position, _Position + _Direction, glm::vec3(0.0f, 1.0f, 0.0f));
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

        _Direction.x = cos(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));
        _Direction.y = sin(glm::radians(_Pitch));
        _Direction.z = sin(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));

        _Projection = glm::perspective(glm::radians(_FOV), _Aspect, _Near, _Far);
        _View = glm::lookAt(_Position, _Position + _Direction, glm::vec3(0.0f, 1.0f, 0.0f));
        _ViewProjection = _Projection * _View;
    }

    PerspectiveCamera(float aspect, float fov, float nearr, float farr)
        : _FOV(fov), _Near(nearr), _Far(farr), _Aspect(aspect), _Exposure(1.0f), _Yaw(0.0f), _Pitch(0.0f), _Roll(0.0f)
    {
        _Direction.x = cos(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));
        _Direction.y = sin(glm::radians(_Pitch));
        _Direction.z = sin(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));

        _Projection = glm::perspective(glm::radians(fov), aspect, nearr, farr);
        _View = glm::lookAt(_Position, _Position + _Direction, glm::vec3(0.0f, 1.0f, 0.0f));
        _ViewProjection = _Projection * _View;
    }

    auto GetExposure() const
    {
        return _Exposure;
    }
    auto const &GetPosition() const
    {
        return _Position;
    }
    auto const &GetDirection() const
    {
        return _Direction;
    }
    auto const &GetProjection() const
    {
        return _Projection;
    }
    auto const &GetViewMatrix() const
    {
        return _View;
    }
    auto const &GetViewProjectionMatrix() const
    {
        return _ViewProjection;
    }
    auto GetYaw() const
    {
        return _Yaw;
    }
    auto GetPitch() const
    {
        return _Pitch;
    }
    auto GetRoll() const
    {
        return _Roll;
    }

    auto SetProjection(float aspect, float fov, float nearr, float farr)
    {
        _Projection = glm::perspective(glm::radians(fov), aspect, nearr, farr);
        _ViewProjection = _Projection * _View;
    }

    auto SetAspect(float aspect)
    {
        _Projection = glm::perspective(glm::radians(_FOV), aspect, _Near, _Far);
        _ViewProjection = _Projection * _View;
    }

    auto SetPosition(glm::vec3 position)
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
#pragma once

#include "PerspectiveCamera.hpp"
#include <opfor/core/base.hpp>
#include <opfor/core/Input.hpp>
#include <opfor/core/types/Vec3.hpp>

namespace opfor
{

class PerspectiveCameraController
{
  private:
    PerspectiveCamera _Camera;

    const float _BaseSpeed = 100.0f;
    const float _FastSpeed = 5000.0f;

    float _MoveSpeed = _BaseSpeed;
    bool _UseInput = true;

  private:
    void UpdateLook(float dt)
    {
        Vec2 vel = Input::GetMouseRelativePosition() * dt * 10.0f;

        float yaw = _Camera.GetYaw();
        float pitch = _Camera.GetPitch();

        yaw += -vel.x;

        pitch += vel.y;
        pitch = std::clamp(pitch, -89.0f, 89.0f);

        _Camera.SetPitch(pitch);
        _Camera.SetYaw(yaw);
    }

    void UpdateMovement(float dt)
    {
        if (_UseInput == false)
            return;

        Vec3 front(Vec3::Normalize(_Camera.GetDirection()));
        Vec3 right(Vec3::Normalize(Vec3::Cross(front, Vec3(0.0f, 1.0f, 0.0f))));
        Vec3 up(Vec3::Cross(right, front));

        // Fast Speed?
        _MoveSpeed =
            Input::GetKey(KeyCode::LeftShift) == KeyStatus::Pressed ? _FastSpeed : _BaseSpeed;

        bool fwd = Input::GetKey(KeyCode::W) == KeyStatus::Pressed;
        bool lft = Input::GetKey(KeyCode::A) == KeyStatus::Pressed;
        bool bck = Input::GetKey(KeyCode::S) == KeyStatus::Pressed;
        bool rgt = Input::GetKey(KeyCode::D) == KeyStatus::Pressed;

        auto position = _Camera.GetPosition();
        position += fwd * dt * _MoveSpeed * front;
        position += bck * dt * _MoveSpeed * -front;
        position += rgt * dt * _MoveSpeed * right;
        position += lft * dt * _MoveSpeed * -right;

        bool moveUp = Input::GetKey(KeyCode::E) == KeyStatus::Pressed;
        bool moveDown = Input::GetKey(KeyCode::Q) == KeyStatus::Pressed;

        position += moveUp * dt * _MoveSpeed * up;
        position += moveDown * dt * _MoveSpeed * -up;

        _Camera.SetPosition(position);
    }

  public:
    PerspectiveCameraController() = default;

    auto &GetCamera()
    {
        return _Camera;
    }

    void Update(float deltaTime)
    {
        auto mouse = Input::GetMouseButton(MouseButton::ButtonRight) == KeyStatus::Pressed;
        if (!mouse)
        {
            return;
        }

        UpdateLook(deltaTime);
        UpdateMovement(deltaTime);
    }
};

} // namespace opfor
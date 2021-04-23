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

    Vec3 _FocusPoint = { 0.f, 0.f, 0.f};
    float _FocusDist = 1000.f;

    float _Yaw = 0.f;
    float _Pitch = 0.f;
    float _Roll = 0.f;

    void UpdateLook(float dt);
    void UpdateOrbit(float dt);
    void UpdateZoom(float dt);
    void UpdateMovement(float dt);
    void FocusCurrentSelection();

  public:
    PerspectiveCameraController() = default;

    void Update(float deltaTime);

    auto &GetCamera()
    {
        return _Camera;
    }

    void SetFocus(Vec3 const &f)
    {
        _FocusPoint = f;
        _FocusDist = Vec3::Magnitude(_Camera.GetPosition() - _FocusPoint);
    }
};

} // namespace opfor
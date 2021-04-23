#include "PerspectiveCameraController.hpp"
#include "opfor/editor/Editor.hpp"
#include "components/TransformComponent.hpp"

namespace opfor
{

void PerspectiveCameraController::Update(float deltaTime)
{
    auto orbit = Input::GetKey(KeyCode::LeftAlt) == KeyStatus::Pressed;
    auto mouseRight = Input::GetMouseButton(MouseButton::ButtonRight) == KeyStatus::Pressed;
    auto mouseLeft = Input::GetMouseButton(MouseButton::ButtonLeft) == KeyStatus::Pressed;

    OP4_CORE_VERBOSE("{} {} {}", math::Degrees(_Yaw), math::Degrees(_Pitch), math::Degrees(_Roll));

    if (!orbit && mouseRight)
    {
        UpdateLook(deltaTime);
        UpdateMovement(deltaTime);
    }
    else if (orbit && mouseLeft)
    {
        UpdateOrbit(deltaTime);
    }
    else if (orbit && mouseRight)
    {
        UpdateZoom(deltaTime);
    }

    if (Input::GetKey(KeyCode::F) == KeyStatus::Pressed)
    {
        FocusCurrentSelection();
    }
}

void PerspectiveCameraController::UpdateLook(float dt)
{
    const Vec2 vel = Input::GetMouseRelativePosition() * dt * 10.0f;

    _Yaw += vel.x;
    _Pitch += vel.y;
    _Pitch = math::Radians(math::Clamp(math::Degrees(_Pitch), -89.f, 89.f));

    _Camera.SetRotation(Quat({_Pitch, _Yaw, _Roll}));
}

void PerspectiveCameraController::UpdateOrbit(float dt)
{
    const Vec2 vel = Input::GetMouseRelativePosition() * dt * 10.0f;

    _Yaw += vel.x;
    _Pitch += vel.y;
    _Pitch = math::Radians(math::Clamp(math::Degrees(_Pitch), -89.f, 89.f));

    const Quat newCameraRotation({_Pitch, _Yaw, _Roll});
    const Vec3 focusedPoint = _Camera.GetPosition() + _Camera.GetForward().Scale(_FocusDist);

    _Camera.SetRotation(newCameraRotation);
    _Camera.SetPosition(focusedPoint - _Camera.GetForward().Scale(_FocusDist));
}

void PerspectiveCameraController::UpdateZoom(float dt)
{
    const float vel = -Input::GetMouseRelativePosition().x * dt * 100.f;

    _Camera.SetPosition(_Camera.GetPosition() + _Camera.GetForward().Scale(vel));
    _FocusDist = (_Camera.GetPosition() - _FocusPoint).Magnitude();
}

void PerspectiveCameraController::UpdateMovement(float dt)
{
    if (_UseInput == false)
        return;

    Vec3 front(Vec3::Normalize(_Camera.GetDirection()));
    Vec3 right(Vec3::Normalize(Vec3::Cross(front, Vec3(0.0f, 1.0f, 0.0f))));
    Vec3 up(Vec3::Cross(right, front));

    // Fast Speed?
    _MoveSpeed = Input::GetKey(KeyCode::LeftShift) == KeyStatus::Pressed ? _FastSpeed : _BaseSpeed;

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

void PerspectiveCameraController::FocusCurrentSelection()
{
    auto const &selection = Editor::Selection();

    if (selection.empty())
        return;

    ecs::IEntityBase *selectedEntity = selection[0];

    if (selectedEntity->HasComponents<TransformComponent>())
    {
        TransformComponent const &transform = selectedEntity->Get<TransformComponent>();
        _FocusPoint = transform.position;
        _FocusDist = (_FocusPoint - _Camera.GetPosition()).Magnitude();
        _Camera.LookAt(_FocusPoint);

        _Yaw = _Camera.GetRotation().Yaw();
        _Pitch = _Camera.GetRotation().Pitch();
        _Roll = _Camera.GetRotation().Roll();
    }
}

}

#pragma once

#include "PerspectiveCamera.hpp"

namespace opfor {

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
			glm::vec2 vel = opfor::Input::GetMouseRelativePosition() * dt * 10.0f;

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

			glm::vec3 front(glm::normalize(_Camera.GetDirection()));
			glm::vec3 right(glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))));
			glm::vec3 up(glm::cross(right, front));

			// Fast Speed?
			_MoveSpeed = opfor::Input::GetKey(opfor::KeyCode::LeftShift) == opfor::KeyStatus::Pressed ? _FastSpeed : _BaseSpeed;

			bool fwd = opfor::Input::GetKey(opfor::KeyCode::W) == opfor::KeyStatus::Pressed;
			bool lft = opfor::Input::GetKey(opfor::KeyCode::A) == opfor::KeyStatus::Pressed;
			bool bck = opfor::Input::GetKey(opfor::KeyCode::S) == opfor::KeyStatus::Pressed;
			bool rgt = opfor::Input::GetKey(opfor::KeyCode::D) == opfor::KeyStatus::Pressed;

			auto position = _Camera.GetPosition();
			position += fwd * dt * _MoveSpeed * front;
			position += bck * dt * _MoveSpeed * -front;
			position += rgt * dt * _MoveSpeed * right;
			position += lft * dt * _MoveSpeed * -right;

			bool moveUp = opfor::Input::GetKey(opfor::KeyCode::E) == opfor::KeyStatus::Pressed;
			bool moveDown = opfor::Input::GetKey(opfor::KeyCode::Q) == opfor::KeyStatus::Pressed;

			position += moveUp * dt * _MoveSpeed * up;
			position += moveDown * dt * _MoveSpeed * -up;

			_Camera.SetPosition(position);
		}

	public:
		PerspectiveCameraController() {}

		auto &GetCamera() { return _Camera; }

		void Update(float deltaTime)
		{
			auto mouse = opfor::Input::GetMouseButton(opfor::MouseButton::ButtonRight) == opfor::KeyStatus::Pressed;
			if (!mouse) {
				return;
			}

			UpdateLook(deltaTime);
			UpdateMovement(deltaTime);
		}
	};

}
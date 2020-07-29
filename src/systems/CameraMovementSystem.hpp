#pragma once

#include "ecs/System.hpp"
#include "lazy.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "engine/core/Input.hpp"
#include "Engine.hpp"

class CameraMovementSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float __unused deltaTime) override
	{
		auto mouse = opfor::Input::GetMouseButton(opfor::MouseButton::ButtonRight) == opfor::KeyStatus::Pressed;
		if (!mouse) {
			return ;
		}

		UpdateLook(deltaTime);
		UpdateMovement(deltaTime);
	}

	void UpdateLook(float dt)
	{
		auto playerCamera = GetEntities<PlayerCameraComponent, TransformComponent>();

		if (playerCamera.size() == 0) {
			return ;
		}

		PlayerCameraComponent camera = playerCamera[0]->Get<PlayerCameraComponent>();
		TransformComponent transform = playerCamera[0]->Get<TransformComponent>();

		if (camera.useInput == false)
			return ;

		glm::vec2 vel = opfor::Input::GetMouseRelativePosition() * dt * 10.0f;

		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(transform.position, transform.position + transform.direction, up);

		transform.yaw += -vel.x;
		transform.pitch += vel.y;

		if (transform.pitch >= 89.0f)
			transform.pitch = 89.0f;
		else if (transform.pitch <= -89.0f)
			transform.pitch = -89.0f;

		transform.direction.x = cos(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));
		transform.direction.y = sin(glm::radians(transform.pitch));
		transform.direction.z = sin(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));;

		camera.view = view;
		camera.viewProjection = camera.projection * camera.view;
		playerCamera[0]->Set(camera);
		playerCamera[0]->Set(transform);
	}

	void UpdateMovement(float dt)
	{
		auto playerCamera = GetEntities<PlayerCameraComponent, TransformComponent>();

		if (playerCamera.size() == 0) {
			return ;
		}

		PlayerCameraComponent camera = playerCamera[0]->Get<PlayerCameraComponent>();
		TransformComponent transform = playerCamera[0]->Get<TransformComponent>();

		if (camera.useInput == false)
			return ;

		glm::vec3 front(glm::normalize(transform.direction));
		glm::vec3 right(glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))));

		bool fwd = opfor::Input::GetKey(opfor::KeyCode::W) == opfor::KeyStatus::Pressed;
		bool lft = opfor::Input::GetKey(opfor::KeyCode::A) == opfor::KeyStatus::Pressed;
		bool bck = opfor::Input::GetKey(opfor::KeyCode::S) == opfor::KeyStatus::Pressed;
		bool rgt = opfor::Input::GetKey(opfor::KeyCode::D) == opfor::KeyStatus::Pressed;

		transform.position += fwd * dt * 100.0f * front;
		transform.position += bck * dt * 100.0f * -front;
		transform.position += rgt * dt * 100.0f * right;
		transform.position += lft * dt * 100.0f * -right;
		playerCamera[0]->Set(transform);
	}
};

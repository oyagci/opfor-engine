#pragma once

#include "ecs/System.hpp"
#include "lazy.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"

class CameraMovementSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float __unused deltaTime) override
	{
		UpdateLook();
		UpdateMovement();
	}

	void UpdateLook()
	{
		auto playerCamera = GetEntities<PlayerCameraComponent, TransformComponent>();

		if (playerCamera.size() == 0) {
			return ;
		}

		PlayerCameraComponent camera = playerCamera[0]->GetComponentData<PlayerCameraComponent>();
		TransformComponent transform = playerCamera[0]->GetComponentData<TransformComponent>();

		glm::vec2 vel = lazy::inputs::input::getMouse().getVelocity() / 10.0f;

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
		playerCamera[0]->SetComponentData<PlayerCameraComponent>(camera);
		playerCamera[0]->SetComponentData<TransformComponent>(transform);
	}

	void UpdateMovement()
	{
		auto playerCamera = GetEntities<PlayerCameraComponent, TransformComponent>();

		if (playerCamera.size() == 0) {
			return ;
		}

		PlayerCameraComponent camera = playerCamera[0]->GetComponentData<PlayerCameraComponent>();
		TransformComponent transform = playerCamera[0]->GetComponentData<TransformComponent>();

		glm::vec3 front(glm::normalize(transform.direction));
		glm::vec3 right(glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))));

		bool fwd = lazy::inputs::input::getKeyboard().getKey(GLFW_KEY_W);
		bool bck = lazy::inputs::input::getKeyboard().getKey(GLFW_KEY_S);
		bool rgt = lazy::inputs::input::getKeyboard().getKey(GLFW_KEY_D);
		bool lft = lazy::inputs::input::getKeyboard().getKey(GLFW_KEY_A);

		transform.position += fwd * 10.0f * front;
		transform.position += bck * 10.0f * -front;
		transform.position += rgt * 10.0f * right;
		transform.position += lft * 10.0f * -right;
		playerCamera[0]->SetComponentData<TransformComponent>(transform);
	}
};
#pragma once

#include "ecs/Component.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct PlayerCameraComponent : ecs::IComponentBase
{
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 viewProjection;

	float exposure;
	bool useInput = true;

	static PlayerCameraComponent New(float fov, float near, float far);
};

inline PlayerCameraComponent PlayerCameraComponent::New(float fov = 90.0f, float near = 0.1f, float far = 5000.0f)
{
	PlayerCameraComponent playerCamera;
		playerCamera.projection = glm::perspective(glm::radians(fov), 16.0f / 9.0f, near, far);
		playerCamera.model = glm::mat4(1.0f);
		playerCamera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		playerCamera.viewProjection = playerCamera.projection * playerCamera.view;
		playerCamera.exposure = 2.0f;

	return playerCamera;
}

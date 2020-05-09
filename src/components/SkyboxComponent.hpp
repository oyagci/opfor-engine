#pragma once

#include "ecs/Component.hpp"

struct SkyboxComponent : ecs::IComponentBase
{
	GLuint vao;
	GLuint vbo;
	GLuint texture;
};

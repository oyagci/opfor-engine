#pragma once

#include "ecs/Component.hpp"

struct SkyboxComponent : ecs::IComponentBase
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint texture = 0;
};

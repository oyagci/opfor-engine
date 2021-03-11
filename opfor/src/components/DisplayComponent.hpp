#pragma once

#include "ecs/Component.hpp"
#include "opfor/core/Window.hpp"
#include "renderer.hpp"

struct DisplayComponent : ecs::IComponentBase
{
    opfor::IWindow *window;
};

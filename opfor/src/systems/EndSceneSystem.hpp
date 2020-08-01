#pragma once

#include "ecs/System.hpp"
#include "opfor/renderer/Renderer.hpp"
#include "opfor/Application.hpp"

class EndSceneSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float) override
	{
		opfor::Renderer::CopyDefaultFramebufferTo(opfor::Application::Get().GetViewport(), opfor::CopyTarget::ColorBufferBit);
		opfor::Renderer::EndScene();
	}
};

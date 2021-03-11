#pragma once

#include "ImGuiLayer.hpp"
#include "Layer.hpp"

namespace opfor
{

class ImGuiWrapperLayer : public Layer
{
  private:
    ImGuiLayer &_instance;

  public:
    ImGuiWrapperLayer() : _instance(ImGuiLayer::Get())
    {
        _DebugName = "ImGuiWrapperLayer";
    }

    void OnAttach() override
    {
        _instance.OnAttach();
    }
    void OnDetach() override
    {
        _instance.OnDetach();
    }
    void OnUpdate(float dt) override
    {
        _instance.OnUpdate(dt);
    }
    void OnEvent(Event &e) override
    {
        _instance.OnEvent(e);
    }
    void OnImGuiRender() override
    {
        _instance.OnImGuiRender();
    }
};

} // namespace opfor

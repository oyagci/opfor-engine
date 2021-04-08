#pragma once

#include "Layer.hpp"
#include <opfor/ecs/Entity.hpp>
#include <opfor/editor/EditorWindow.hpp>
#include "imgui.h"

namespace opfor
{

class ImGuiLayer : public Layer
{
  private:
    ImVec2 _ViewportPosition;
    ImVec2 _ViewportSize;

    bool _logAutoScroll = true;
    size_t _SelectedItem = 0;

    ecs::IEntityBase *_currentEntity = nullptr;

    void BeginFrame();
    void EndFrame();

    void BeginDockspace();
    void EndDockspace();

    void SetupImGuiStyle();

    Vector<UniquePtr<IEditorWindow>> _windows;

  private:
    ImGuiLayer();

  public:
    static ImGuiLayer &Get()
    {
        static ImGuiLayer instance;
        return instance;
    }

    void OnAttach() override;
    void OnDetach() override;
    void OnImGuiRender() override;

    ImVec2 GetViewportSize() const
    {
        return _ViewportSize;
    }
    ImVec2 GetViewportPosition() const
    {
        return _ViewportPosition;
    }

    [[nodiscard]] ecs::IEntityBase *GetSelectedEntity() const
    {
        return _currentEntity;
    }
    void SetSelectedEntity(ecs::IEntityBase *ent)
    {
        _currentEntity = ent;
    }

    template <typename T> void OpenWindow()
    {
        static_assert(std::is_base_of<IEditorWindow, T>::value);
        _windows.emplace_back(MakeUnique<T>());
    }
};

} // namespace opfor

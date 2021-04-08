#pragma once
#include <Refureku/Refureku.h>
#include <opfor/ecs/ecs.hpp>
#include <generated/EditorGUI.rfk.h>

namespace opfor OP4NAMESPACE()
{

/**
 * @brief An interface used to describe how to render an entity in the editor
 */
class OP4CLASS() EditorGUI : public rfk::Object
{
  public:
    virtual ~EditorGUI() override = default;

    virtual void OnToolGUI()
    {
    }

    /**
     * @brief The current instance of the entity being rendered
     */
    ecs::IComponentBase *target = nullptr;

    EditorGUI_GENERATED
};

} // namespace opfor

File_GENERATED

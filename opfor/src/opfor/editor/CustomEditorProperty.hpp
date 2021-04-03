#pragma once
#include <Refureku/Refureku.h>
#include <Refureku/TypeInfo/Properties/PropertySettings.h>
#include "EditorGUI.hpp"
#include <generated/CustomEditorProperty.rfk.h>

/**
* @brief A property to attach a custom in-editor layout to the component this property is added
*/
class OP4CLASS(PropertySettings(rfk::EEntityKind::Class | rfk::EEntityKind::Struct)) CustomEditor : public rfk::Property
{
  public:

    /**
     * @param [in] editorGUI    The user-defined custom editor to use
     */
    CustomEditor(EditorGUI *editorGUI) : _editorGUI(editorGUI)
    {
    }

    CustomEditor() = delete;
    CustomEditor(CustomEditor const &) = delete;
    CustomEditor(CustomEditor const &&) = delete;
    virtual ~CustomEditor() = default;

    CustomEditor &operator=(CustomEditor const &) = delete;
    CustomEditor &operator=(CustomEditor const &&) = delete;

    void operator()() const
    {
        _editorGUI->OnToolGUI();
    }

    [[nodiscard]] auto GUI() const { return _editorGUI.get(); }

  private:
    opfor::UniquePtr<EditorGUI> _editorGUI;

    CustomEditor_GENERATED
};

File_GENERATED

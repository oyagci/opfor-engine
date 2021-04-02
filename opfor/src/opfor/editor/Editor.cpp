#include <opfor/core/Application.hpp>
#include <opfor/editor/Editor.hpp>

namespace opfor
{

void Editor::SelectEntity(uuids::uuid const &uuid)
{
    auto selection = opfor::Application::Get().GetEntity(uuid);

    _selectedEntities.clear();
    if (selection)
    {
        _selectedEntities.push_back(*selection);
    }
}

Vector<ecs::IEntityBase *> const &Editor::Selection()
{
    return _selectedEntities;
}

} // namespace opfor

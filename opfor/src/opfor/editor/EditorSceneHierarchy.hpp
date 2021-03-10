#pragma once
#include "EditorWindow.hpp"
#include "opfor/core/base.hpp"
#include "uuid.h"
#include <optional>

class EditorSceneHierarchy : public IEditorWindow
{
private:
	struct EntityHierarchy
	{
		opfor::String name;
		uuids::uuid uuid;
		opfor::Optional<uuids::uuid> parent = std::nullopt;
		opfor::Vector<EntityHierarchy> children;
	};

private:
	int _selectedItem = 0;

	void DrawHierarchy(opfor::UnorderedMap<uuids::uuid, EntityHierarchy> const &lookup, EntityHierarchy const &hierarchy) const;

public:
	void OnDrawGUI() override;
};

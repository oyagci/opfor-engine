#pragma once

#include <string>
#include <map>
#include <optional>
#include <memory>
#include "UIScene.hpp"
#include "lazy.hpp"

using lazy::graphics::Shader;

class UI
{
private:
	struct UIState {
		std::string sceneName;
		std::shared_ptr<IUIScene> currentScene;
	};

	enum UIAction {
		CHANGE_SCENE,
	};
public:
	UI(float width, float height);

	void update();
	void render();

	/*
	 * Call a registered callback from a UI component
	 */
	void call(std::string const &funcName);

	/*
	 * Register a callback to be used by a UI component
	 */
	void registerFunc(std::string const &name, std::function<void()>);

	void showScene(std::string const &sceneName);

private:
	void action(UIAction action, std::string const &);

	std::optional<std::shared_ptr<IUIScene>> getScene(std::string const &name);

	template<class T,
	typename = std::enable_if_t<std::is_base_of<IUIScene, T>::value>>
	bool loadScene(std::string const &name);

	void renderScene(IUIScene &scene);

	void renderComponents(std::vector<std::shared_ptr<ASceneComponent>> components,
		ASceneComponent *parent = nullptr, glm::vec2 parentPos = glm::vec2(0.0f, 0.0f));

	void updateComponents(std::vector<std::shared_ptr<ASceneComponent>> components);

private:
	UIState _state;
	std::map<std::string, std::shared_ptr<IUIScene>> _scenes;
	Shader _shader;
	std::map<std::string, std::function<void()>> _callbacks;
	glm::vec2 _size;
};

#pragma once

#include "lazy.hpp"
#include "TextRenderer.hpp"
#include "Anchor.hpp"
#include "SceneComponent.hpp"
#include "Label.hpp"

using lazy::graphics::Mesh;
using namespace anchor;

class Button : public lazy::inputs::IMouseObserver, public ASceneComponent
{
public:
	Button(IUIScene *scene);
	~Button();

	void update() override;
	void draw(Shader &shader) override;

	void setText(std::string const &text);

	// IMouseObserver member functions
	void onClickUpInside() override;
	void onHover(bool) override;
	glm::vec4 getObservedArea() const override;
	bool canBeClicked() const override { return _canBeClicked; }
	void setCanBeClicked(bool val) { _canBeClicked = val; }

	std::function<void()> _onClick;

private:
	void setup(glm::vec2 size, Anchor anchorPoint);
	void buildMesh();

	Mesh _mesh;
	std::string _text;
	bool _canBeClicked;
	bool _isHovering;
	std::shared_ptr<Label> _label;
};

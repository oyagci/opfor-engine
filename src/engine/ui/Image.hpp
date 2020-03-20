#pragma once

#include "SceneComponent.hpp"
#include <glm/glm.hpp>
#include "lazy.hpp"
#include "TextureManager.hpp"

using lazy::graphics::Mesh;

class Image : public ASceneComponent
{
public:
	Image(IUIScene *parent) : ASceneComponent(parent)
	{
	}

	void setImage(float width, float height, std::string const &path)
	{
		TextureManager::instance().createTexture(path, path, {
			{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
			{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
			{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		});
		// TODO: use a UUID rather than the path
		_textureName = path;

		_quad = Mesh();
		setSize(glm::vec2(width, height));
		std::array<glm::vec3, 4> verts = {
			glm::vec3(-(width / 2.0f), -(height / 2.0f), 0.0f),
			glm::vec3( (width / 2.0f), -(height / 2.0f), 0.0f),
			glm::vec3( (width / 2.0f),  (height / 2.0f), 0.0f),
			glm::vec3(-(width / 2.0f),  (height / 2.0f), 0.0f),
		};

		std::array<glm::vec3, 4> tex = {
			glm::vec3(  0.0f,   0.0f, 0.0f ),
			glm::vec3(  1.0f,   0.0f, 0.0f ),
			glm::vec3(  1.0f,   1.0f, 0.0f ),
			glm::vec3(  0.0f,   1.0f, 0.0f ),
		};

		for (auto const &v : verts) {
			_quad.addPosition(v);
		}
		for (auto const &t : tex) {
			_quad.addUv(t);
		}

		_quad.addTriangle(glm::uvec3(0, 1, 2))
			 .addTriangle(glm::uvec3(0, 2, 3));
		_quad.build();
	}

	void draw(Shader &shader) override
	{
		TextureManager::instance().bind(_textureName, GL_TEXTURE0);
		_quad.draw();
	}

private:
	Mesh _quad;
	std::string _textureName;
};

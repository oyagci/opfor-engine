#pragma once

#include "lazy.hpp"
#include <vector>

namespace engine
{

class Mesh
{
public:
	enum class TextureType {
		TT_Diffuse,
		TT_Specular,
		TT_Normal,
	};

	struct Texture {
		std::string name;
		TextureType type;
	};

private:
	std::vector<GLfloat> vPositions;
	std::vector<GLfloat> vNormals;
	std::vector<GLfloat> vUvs;
	std::vector<GLfloat> vTangents;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	GLuint vao;
	GLuint ibo;
	GLuint objectBuffer;

public:
	Mesh();
	~Mesh();
	Mesh(Mesh &&);
	Mesh &operator=(Mesh &&);

	Mesh &addPosition(const glm::vec3 &v);
	Mesh &addNormal(const glm::vec3 &v);
	Mesh &addUv(const glm::vec2 &v);
	Mesh &addTangent(const glm::vec3 &v);
	Mesh &addIndex(const int index);
	Mesh &addTriangle(const glm::u32vec3 &triangle);
	Mesh &addTexture(std::string const &name, TextureType type);

	Mesh &build();

	void draw() const;
	std::vector<Texture> const &getTextures() const;
};

}

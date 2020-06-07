#pragma once

#include "lazy.hpp"
#include <vector>
#include "IDrawable.hpp"
#include "Material.hpp"

namespace engine
{

class Mesh : public IDrawable
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
	GLuint lightMap;

	std::string _material;
	std::optional<std::string> _pbrMaterial;

	void InitLightmap();

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

	std::vector<GLfloat> const &GetPositions() const { return vPositions; }
	std::vector<GLfloat> const &GetNormals() const { return vNormals; }
	std::vector<GLfloat> const &GetUVs() const { return vUvs; }
	std::vector<GLfloat> const &GetTangents() const { return vTangents; }
	std::vector<GLuint>  const &GetIndices() const { return indices; }
	std::vector<GLuint>  const GetTextureIDs() const;
	GLuint GetLightmap() const { return lightMap; }

	void SetMaterial(std::string name) { _material = name; }
	std::string GetMaterial() const { return _material; }

	void SetPbrMaterial(std::string name) { _pbrMaterial = name; }
	std::optional<std::string> GetPbrMaterial() { return _pbrMaterial; }

	Mesh &build();

	void Draw() const override;
	std::vector<Texture> const &getTextures() const;
};

}

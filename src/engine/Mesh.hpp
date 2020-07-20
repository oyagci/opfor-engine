#pragma once

#include <lazy.hpp>
#include <vector>
#include <list>
#include "IDrawable.hpp"
#include "Material.hpp"
#include "engine/renderer/Buffer.hpp"

namespace opfor
{

struct MeshVertexData
{
	std::array<float, 3> Position;
	std::array<float, 3> Normal;
	std::array<float, 2> Uv;
	std::array<float, 4> Tangent;
};

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
	std::list<std::array<float, 3>> vPositions;
	std::list<std::array<float, 3>> vNormals;
	std::list<std::array<float, 2>> vUvs;
	std::list<std::array<float, 4>> vTangents;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	GLuint vao;

	UniquePtr<VertexBuffer> _vertexBuffer;
	UniquePtr<IndexBuffer> _indexBuffer;

	std::string _material;
	std::optional<std::string> _pbrMaterial;

public:
	Mesh();
	~Mesh();
	Mesh(Mesh &&);
	Mesh &operator=(Mesh &&);

	Mesh &addPosition(const glm::vec3 &v);
	Mesh &addNormal(const glm::vec3 &v);
	Mesh &addUv(const glm::vec2 &v);
	Mesh &addTangent(const glm::vec4 &v);
	Mesh &addIndex(const int index);
	Mesh &addTriangle(const glm::uvec3 &triangle);
	Mesh &addTexture(std::string const &name, TextureType type);

	auto const &GetPositions() const { return vPositions; }
	auto const &GetNormals() const { return vNormals; }
	auto const &GetUVs() const { return vUvs; }
	auto const &GetTangents() const { return vTangents; }
	auto const &GetIndices() const { return indices; }
	auto GetTextureIDs() const;

	void SetMaterial(std::string name) { _material = name; }
	std::string GetMaterial() const { return _material; }

	void SetPbrMaterial(std::string name) { _pbrMaterial = name; }
	std::optional<std::string> GetPbrMaterial() { return _pbrMaterial; }

	Mesh &build();

	void Draw() const override;
	std::vector<Texture> const &getTextures() const;
};

}

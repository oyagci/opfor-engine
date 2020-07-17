#include "Mesh.hpp"
#include <fmt/format.h>
#include "TextureManager.hpp"

namespace opfor
{
	Mesh::Mesh() : vao(0), ibo(0), objectBuffer(0), lightMap(0)
	{
		addTexture("prototype_tile_8", TextureType::TT_Diffuse);
	}

	Mesh::~Mesh()
	{
		glDeleteBuffers(1, &ibo);
		glDeleteBuffers(1, &objectBuffer);
		glDeleteVertexArrays(1, &vao);
		glDeleteTextures(1, &lightMap);
	}

	Mesh::Mesh(Mesh &&m)
	{
		vPositions = std::move(m.vPositions);
		vNormals = std::move(m.vNormals);
		vUvs = std::move(m.vUvs);
		vTangents = std::move(m.vTangents);
		indices = std::move(m.indices);
		textures = std::move(m.textures);
		_material = std::move(m._material);

		vao = m.vao;
		m.vao = 0;

		objectBuffer = m.objectBuffer;
		m.objectBuffer = 0;

		ibo = m.ibo;
		m.ibo = 0;

		lightMap = m.lightMap;
		m.lightMap = 0;

		_pbrMaterial = std::move(m._pbrMaterial);
		m._pbrMaterial.reset();
	}

	Mesh &Mesh::operator=(Mesh &&rhs)
	{
		if (this != &rhs)
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &objectBuffer);
			glDeleteTextures(1, &lightMap);

			vPositions = std::move(rhs.vPositions);
			vNormals = std::move(rhs.vNormals);
			vUvs = std::move(rhs.vUvs);
			vTangents = std::move(rhs.vTangents);
			indices = std::move(rhs.indices);
			textures = std::move(rhs.textures);
			_material = std::move(rhs._material);

			vao = rhs.vao;
			rhs.vao = 0;

			objectBuffer = rhs.objectBuffer;
			rhs.objectBuffer = 0;

			ibo = rhs.ibo;
			rhs.ibo = 0;

			lightMap = rhs.lightMap;
			rhs.lightMap = 0;

			_pbrMaterial = rhs._pbrMaterial;
			rhs._pbrMaterial.reset();
		}
		return *this;
	}

	Mesh &Mesh::addPosition(const glm::vec3 &v)
	{
		vPositions.push_back(v.x);
		vPositions.push_back(v.y);
		vPositions.push_back(v.z);

		return *this;
	}

	Mesh &Mesh::addNormal(const glm::vec3 &v)
	{
		vNormals.push_back(v.x);
		vNormals.push_back(v.y);
		vNormals.push_back(v.z);

		return *this;
	}

	Mesh &Mesh::addUv(const glm::vec2 &v)
	{
		vUvs.push_back(v.x);
		vUvs.push_back(v.y);

		return *this;
	}

	Mesh &Mesh::addTangent(const glm::vec4 &v)
	{
		vTangents.push_back(v.x);
		vTangents.push_back(v.y);
		vTangents.push_back(v.z);
		vTangents.push_back(v.w);

		return *this;
	}

	Mesh &Mesh::addIndex(const int index)
	{
		indices.push_back(index);

		return *this;
	}

	Mesh &Mesh::addTriangle(const glm::u32vec3 &triangle)
	{
		indices.push_back(triangle.x);
		indices.push_back(triangle.y);
		indices.push_back(triangle.z);

		return *this;
	}

	Mesh &Mesh::addTexture(std::string const &name, TextureType type)
	{
		textures.push_back(Texture { name, type });

		return *this;
	}

	std::vector<Mesh::Texture> const &Mesh::getTextures() const
	{
		return textures;
	}

	Mesh &Mesh::build()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		const size_t bufferSize = (vPositions.size()
								  + vNormals.size()
								  + vUvs.size()
								  + vTangents.size()) * sizeof(GLfloat);
		size_t offset = 0;

		glGenBuffers(1, &objectBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, objectBuffer);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
		if (vPositions.size() > 0) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, vPositions.size() * sizeof(GLfloat), vPositions.data());
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
			offset += vPositions.size() * sizeof(GLfloat);
		}
		if (vNormals.size() > 0) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, vNormals.size() * sizeof(GLfloat), vNormals.data());
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
			offset += vNormals.size() * sizeof(GLfloat);
		}
		if (vUvs.size() > 0) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, vUvs.size() * sizeof(GLfloat), vUvs.data());
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
			offset += vUvs.size() * sizeof(GLfloat);
		}
		if (vTangents.size() > 0) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, vTangents.size() * sizeof(GLfloat), vTangents.data());
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
			offset += vTangents.size() * sizeof(GLfloat);
		}

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);

		return *this;
	}

	void Mesh::Draw() const
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	std::vector<GLuint> const Mesh::GetTextureIDs() const
	{
		std::vector<GLuint> ids(textures.size());

		for (auto const &t : textures) {
			GLuint id = TextureManager::Get().get(t.name);
			ids.push_back(id);
		}

		return ids;
	}

	void Mesh::InitLightmap()
	{
		const glm::uvec2 LightmapSize{ 1024, 1024 };

		glGenTextures(1, &lightMap);
		glBindTexture(GL_TEXTURE_2D, lightMap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, LightmapSize.x, LightmapSize.y, 0, GL_RED, GL_FLOAT, nullptr);
	}
	
}

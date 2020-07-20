#include <lazy.hpp>
#include "Mesh.hpp"
#include "TextureManager.hpp"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "engine/renderer/Buffer.hpp"

namespace opfor
{

	Mesh::Mesh() : vao(0)
	{
		addTexture("prototype_tile_8", TextureType::TT_Diffuse);
	}

	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &vao);
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

		_indexBuffer = std::move(m._indexBuffer);
		_vertexBuffer = std::move(m._vertexBuffer);

		_pbrMaterial = std::move(m._pbrMaterial);
		m._pbrMaterial.reset();
	}

	Mesh &Mesh::operator=(Mesh &&rhs)
	{
		if (this != &rhs)
		{
			glDeleteVertexArrays(1, &vao);

			vPositions = std::move(rhs.vPositions);
			vNormals = std::move(rhs.vNormals);
			vUvs = std::move(rhs.vUvs);
			vTangents = std::move(rhs.vTangents);
			indices = std::move(rhs.indices);
			textures = std::move(rhs.textures);
			_material = std::move(rhs._material);

			vao = rhs.vao;
			rhs.vao = 0;

			_indexBuffer = std::move(rhs._indexBuffer);
			_vertexBuffer = std::move(rhs._vertexBuffer);

			_pbrMaterial = rhs._pbrMaterial;
			rhs._pbrMaterial.reset();
		}
		return *this;
	}

	Mesh &Mesh::addPosition(const glm::vec3 &v)
	{
		vPositions.push_back({ v.x, v.y, v.z });

		return *this;
	}

	Mesh &Mesh::addNormal(const glm::vec3 &v)
	{
		vNormals.push_back({ v.x, v.y, v.z });

		return *this;
	}

	Mesh &Mesh::addUv(const glm::vec2 &v)
	{
		vUvs.push_back({ v.x, v.y });

		return *this;
	}

	Mesh &Mesh::addTangent(const glm::vec4 &v)
	{
		vTangents.push_back({ v.x, v.y, v.z, v.w });

		return *this;
	}

	Mesh &Mesh::addIndex(const int index)
	{
		indices.push_back(index);

		return *this;
	}

	Mesh &Mesh::addTriangle(const glm::uvec3 &triangle)
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

		if (vPositions.size() > vNormals.size()) vNormals.resize(vPositions.size(), { 0.0f, 0.0f, 1.0f });
		if (vPositions.size() > vUvs.size()) vUvs.resize(vPositions.size());
		if (vPositions.size() > vTangents.size()) vTangents.resize(vPositions.size());

		OP4_CORE_ASSERT(vPositions.size() == vNormals.size() &&
						vPositions.size() == vUvs.size() &&
						vPositions.size() == vTangents.size(),
			"Vertex Data is incomplete\n");

		std::vector<MeshVertexData> vertices;

		const size_t count = vPositions.size();
		vertices.reserve(count);

		for (size_t i = 0; i < count; i++) {

			auto const &position = vPositions.front();
			auto const &normal = vNormals.front();
			auto const &tex_coord = vUvs.front();
			auto const &tangent = vTangents.front();

			MeshVertexData vertexData = { position, normal, tex_coord, tangent };

			vertices.push_back(vertexData);

			vPositions.pop_front();
			vNormals.pop_front();
			vUvs.pop_front();
			vTangents.pop_front();
		}

		_vertexBuffer = VertexBuffer::Create(reinterpret_cast<float*>(vertices.data()), sizeof(MeshVertexData) * vertices.size());
		_indexBuffer = IndexBuffer::Create(reinterpret_cast<uint32_t*>(indices.data()), indices.size());

		{
			BufferLayout layout = {
				{ ShaderDataType::Float3, "in_position" },
				{ ShaderDataType::Float3, "in_normal"   },
				{ ShaderDataType::Float2, "tex_coords"  },
				{ ShaderDataType::Float4, "in_tangent"  },
			};

			_vertexBuffer->SetLayout(layout);
		}

		_vertexBuffer->Bind();
		_indexBuffer->Bind();

		auto const &layout = _vertexBuffer->GetLayout();
		auto elements = _vertexBuffer->GetLayout().GetElements();

		for (size_t i = 0; i < elements.size(); i++) {

			auto const &elem = elements[i];

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, elem.GetComponentCount(), ShaderDataTypeToOpenGLBase(elem.Type), elem.Normalized,
				layout.GetStride(), reinterpret_cast<void*>(elem.Offset));
		}

		glBindVertexArray(0);

		return *this;
	}

	void Mesh::Draw() const
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, _indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	auto Mesh::GetTextureIDs() const
	{
		std::vector<GLuint> ids(textures.size());

		for (auto const &t : textures) {
			GLuint id = TextureManager::Get().get(t.name);
			ids.push_back(id);
		}

		return ids;
	}
}

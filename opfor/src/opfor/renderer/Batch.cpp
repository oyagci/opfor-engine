#include <renderer.hpp>
#include "Batch.hpp"
#include "opfor/core/Application.hpp"

namespace opfor {

Batch::Batch() : _vao(0), _ibo(0)
{
}

void Batch::AddMesh(
	std::vector<GLfloat> const &positions,
	std::vector<GLfloat> const &normals,
	std::vector<GLfloat> const &uvs,
	std::vector<GLfloat> const &tangents,
	std::string const &material,
	std::vector<GLuint> const &indices
)
{
	_positions.insert(_positions.end(), positions.begin(), positions.end());
	_normals.insert(_normals.end(), normals.begin(), normals.end());
	_uvs.insert(_uvs.end(), uvs.begin(), uvs.end());
	_tangents.insert(_tangents.end(), tangents.begin(), tangents.end());
	_materials.push_back(material);
	_indices.insert(_indices.end(), indices.begin(), indices.end());
}

void Batch::Build()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	const size_t bufferSize = (_positions.size()
						      + _normals.size()
						      + _uvs.size()
						      + _tangents.size()
						      + _materials.size()) * sizeof(GLfloat);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	size_t offset = 0;

	if (_positions.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _positions.size() * sizeof(GLfloat), _positions.data());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _positions.size() * sizeof(GLfloat);
	}
	if (_normals.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _normals.size() * sizeof(GLfloat), _normals.data());
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _normals.size() * sizeof(GLfloat);
	}
	if (_uvs.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _uvs.size() * sizeof(GLfloat), _uvs.data());
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _uvs.size() * sizeof(GLfloat);
	}
	if (_tangents.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _tangents.size() * sizeof(GLfloat), _tangents.data());
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _tangents.size() * sizeof(GLfloat);
	}
//	if (_materials.size() > 0) {
//		glBufferSubData(GL_ARRAY_BUFFER, offset, _materials.size() * sizeof(GLfloat), _materials.data());
//		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
//		offset += _materials.size() * sizeof(GLfloat);
//	}

	std::vector<GLfloat> _materialIds{};
	_materialIds.reserve(_materials.size());
	std::transform(_materials.begin(), _materials.end(), _materialIds.begin(),
		[] (std::string const &m) -> GLfloat { return opfor::Application::Get().GetMaterialId(m); });

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), _indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Batch::Draw() const
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

}

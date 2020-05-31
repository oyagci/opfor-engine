#include "Batch.hpp"
#include "Mesh.hpp"

namespace engine {

Batch::Batch() : _vao(0), _ibo(0)
{
}

void Batch::AddMesh(const Mesh &m)
{
	auto positions = m.GetPositions();
	_batchMesh.positions.insert(_batchMesh.positions.end(), positions.begin(), positions.end());

	auto normals = m.GetNormals();
	_batchMesh.normals.insert(_batchMesh.normals.end(), positions.begin(), positions.end());

	auto uvs = m.GetUVs();
	_batchMesh.uvs.insert(_batchMesh.uvs.end(), positions.begin(), positions.end());

	auto tangents = m.GetTangents();
	_batchMesh.tangents.insert(_batchMesh.tangents.end(), tangents.begin(), tangents.end());

	auto textures = std::vector(m.GetTextureIDs());
	std::vector<GLfloat> fTextures(textures.size());
	std::transform(textures.begin(), textures.end(), fTextures.begin(),
		[&] (GLuint tex) -> GLfloat { return static_cast<GLfloat>(tex);
	});
	_batchMesh.materials.insert(_batchMesh.materials.end(), fTextures.begin(), fTextures.end());

	auto indices = std::vector(m.GetIndices());
	std::transform(indices.begin(), indices.end(), indices.begin(),
		[&] (GLuint indice) -> GLuint { return indice + _batchMesh.positions.size();
	});
	_batchMesh.indices.insert(_batchMesh.indices.end(), indices.begin(), indices.end());
}

void Batch::Build()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	const size_t bufferSize = (_batchMesh.positions.size()
						      + _batchMesh.normals.size()
						      + _batchMesh.uvs.size()
						      + _batchMesh.tangents.size()
						      + _batchMesh.materials.size()) * sizeof(GLfloat);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	size_t offset = 0;

	if (_batchMesh.positions.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _batchMesh.positions.size() * sizeof(GLfloat), _batchMesh.positions.data());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _batchMesh.positions.size() * sizeof(GLfloat);
	}
	if (_batchMesh.normals.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _batchMesh.normals.size() * sizeof(GLfloat), _batchMesh.normals.data());
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _batchMesh.normals.size() * sizeof(GLfloat);
	}
	if (_batchMesh.uvs.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _batchMesh.uvs.size() * sizeof(GLfloat), _batchMesh.uvs.data());
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _batchMesh.uvs.size() * sizeof(GLfloat);
	}
	if (_batchMesh.tangents.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _batchMesh.tangents.size() * sizeof(GLfloat), _batchMesh.tangents.data());
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _batchMesh.tangents.size() * sizeof(GLfloat);
	}
	if (_batchMesh.materials.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, _batchMesh.materials.size() * sizeof(GLfloat), _batchMesh.materials.data());
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), reinterpret_cast<void*>(offset));
		offset += _batchMesh.materials.size() * sizeof(GLfloat);
	}

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glBindVertexArray(0);
}

void Batch::Draw()
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _batchMesh.indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

}

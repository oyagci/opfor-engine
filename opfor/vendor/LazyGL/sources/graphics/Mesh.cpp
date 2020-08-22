//
// Created by Marc on 01/06/2018.
//

#include "Mesh.hpp"


namespace lazy
{
	namespace graphics
	{
		Mesh::Mesh()
		{
			vao = 0;
			vbo = 0;
			nbo = 0;
			ubo = 0;
			tbo = 0;
			ibo = 0;
		}

		Mesh::~Mesh()
		{
			glDeleteBuffers(1, &ibo);
			glDeleteBuffers(1, &tbo);
			glDeleteBuffers(1, &ubo);
			glDeleteBuffers(1, &nbo);
			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
		}

		Mesh::Mesh(Mesh &&m)
		{
			vPositions = std::move(m.vPositions);
			vNormals = std::move(m.vNormals);
			vUvs = std::move(m.vUvs);
			vTangents = std::move(m.vTangents);
			indices = std::move(m.indices);

			vao = m.vao;
			vbo = m.vbo;
			nbo = m.nbo;
			ubo = m.ubo;
			tbo = m.tbo;
			ibo = m.ibo;

			m.vao = 0;
			m.vbo = 0;
			m.nbo = 0;
			m.ubo = 0;
			m.tbo = 0;
			m.ibo = 0;
		}

		Mesh &Mesh::operator=(Mesh &&rhs)
		{
			if (this != &rhs)
			{
				glDeleteBuffers(1, &ibo);
				glDeleteBuffers(1, &tbo);
				glDeleteBuffers(1, &ubo);
				glDeleteBuffers(1, &nbo);
				glDeleteBuffers(1, &vbo);
				glDeleteVertexArrays(1, &vao);

				vPositions = std::move(rhs.vPositions);
				vNormals = std::move(rhs.vNormals);
				vUvs = std::move(rhs.vUvs);
				vTangents = std::move(rhs.vTangents);
				indices = std::move(rhs.indices);

				vao = rhs.vao;
				vbo = rhs.vbo;
				nbo = rhs.nbo;
				ubo = rhs.ubo;
				tbo = rhs.tbo;
				ibo = rhs.ibo;

				rhs.vao = 0;
				rhs.vbo = 0;
				rhs.nbo = 0;
				rhs.ubo = 0;
				rhs.tbo = 0;
				rhs.ibo = 0;
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

		Mesh &Mesh::addTangent(const glm::vec3 &v)
		{
			vTangents.push_back(v.x);
			vTangents.push_back(v.y);
			vTangents.push_back(v.z);

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

		Mesh &Mesh::build()
		{
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &nbo);
			glGenBuffers(1, &ubo);
			glGenBuffers(1, &tbo);
			glGenBuffers(1, &ibo);

			glBindVertexArray(vao);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vPositions.size(), (void *)(&vPositions[0]), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

			glBindBuffer(GL_ARRAY_BUFFER, nbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vNormals.size(), &vNormals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

			glBindBuffer(GL_ARRAY_BUFFER, ubo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vUvs.size(), &vUvs[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

			glBindBuffer(GL_ARRAY_BUFFER, tbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vTangents.size(), &vTangents[0], GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

			glBindVertexArray(0);

			return *this;
		}

		void Mesh::draw() const
		{
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
	}
}

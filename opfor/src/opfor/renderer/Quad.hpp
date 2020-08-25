#pragma once

#include <array>
#include "Mesh.hpp"

namespace opfor {

	class Quad
	{
	private:
		Mesh _Mesh;

	public:
		Quad()
		{
			std::array<glm::vec3, 4> pos = {
				glm::vec3(-1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f,  1.0f, 0.0f),
				glm::vec3(-1.0f,  1.0f, 0.0f),
			};
			std::array<glm::vec2, 4> tex = {
				glm::vec2(0.0f, 0.0f),
				glm::vec2(1.0f, 0.0f),
				glm::vec2(1.0f, 1.0f),
				glm::vec2(0.0f, 1.0f),
			};

			for (auto p : pos) {
				_Mesh.addPosition(p);
			}
			for (auto t : tex) {
				_Mesh.addUv(t);
			}
			_Mesh.addTriangle(glm::uvec3(0, 1, 2));
			_Mesh.addTriangle(glm::uvec3(0, 2, 3));
			_Mesh.build();

		}

		void Draw()
		{
			Renderer::Submit(_Mesh.GetVertexArray());
		}
	};
}
//
// Created by Marc on 01/06/2018.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace lazy
{
	namespace maths
	{
		struct transform
		{
		public:
			glm::vec3	position;
			glm::quat	rotation;
			glm::vec3	scale;
			transform	*parent;

			glm::mat4	toMatrix() const
			{
				glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
				glm::mat4 rotationMatrix = glm::toMat4(rotation);
				glm::mat4 scaleMatrix = glm::scale(glm::mat4(), scale);
				glm::mat4 result = translationMatrix * rotationMatrix * scaleMatrix;

				if (parent)
					result *= parent->toMatrix();

				return result;
			}

			glm::mat4	toCameraMatrix() const
			{
				glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(-position.x, -position.y, -position.z));
				glm::mat4 rotationMatrix = glm::toMat4(rotation);
				glm::mat4 result = rotationMatrix * translationMatrix;

				if (parent)
					result *= parent->toMatrix();

				return result;
			}

			void		translate(const glm::vec3 &dir)
			{
				position += dir;
			}

			void		rotate(float rad, const glm::vec3 &axis)
			{
				rotation = glm::rotate(rotation, rad, axis);
			}

			glm::vec3	forward() const
			{
				glm::vec4 dir(0.0f, 0.0f, -1.0f, 0.0f);
				
				return (glm::vec3(dir * glm::toMat4(rotation)));
			}

			glm::vec3	back() const
			{
				glm::vec4 dir(0.0f, 0.0f, 1.0f, 0.0f);

				return (glm::vec3(dir * glm::toMat4(rotation)));
			}

			glm::vec3	left() const
			{
				glm::vec4 dir(-1.0f, 0.0f, 0.0f, 0.0f);

				return (glm::vec3(dir * glm::toMat4(rotation)));
			}

			glm::vec3	right() const
			{
				glm::vec4 dir(1.0f, 0.0f, 0.0f, 0.0f);

				return (glm::vec3(dir * glm::toMat4(rotation)));
			}

			glm::vec3	top() const
			{
				glm::vec4 dir(0.0f, 1.0f, 0.0f, 0.0f);

				return (glm::vec3(dir * glm::toMat4(rotation)));
			}

			glm::vec3	bottom() const
			{
				glm::vec4 dir(0.0f, -1.0f, 0.0f, 0.0f);

				return (glm::vec3(dir * glm::toMat4(rotation)));
			}
		};
	}
}

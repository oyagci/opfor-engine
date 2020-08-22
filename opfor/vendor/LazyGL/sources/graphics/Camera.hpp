//
// Created by Marc on 01/06/2018.
//

#pragma once

#include <array>
#include "maths/transform.hpp"
#include "Display.hpp"

namespace lazy
{
	namespace graphics
	{
		class Camera
		{
		private:
			enum Planes {
				TOP 	= 0,
				BOTTOM	= 1,
				LEFT	= 2,
				RIGHT	= 3,
				FAR		= 4,
				NEAR	= 5,
			};
			
		private:
			const Display		&display;
			maths::transform	transform;

			float										fov;
			float										aspect;
			float										near;
			float										far;
			glm::mat4									projection;
			std::array<std::pair<glm::vec3, float>, 6>	frustumPlanes;

			void updateFrustum();

		public:
			Camera(const Display &display, const maths::transform &trs);
			~Camera();

			void input(float moveSpeed, float mouseSpeed, std::array<GLuint, 6> keys);
			void update();

			void setProjection(float fov, float near, float far);
			void updateProjection();
			void setPosition(glm::vec3 pos);

			glm::mat4 getViewMatrix() const { return transform.toCameraMatrix(); }
			glm::mat4 getProjectionMatrix() const { return projection; }
			glm::mat4 getViewProjectionMatrix() const { return (projection * transform.toCameraMatrix()); }
			glm::vec3 getPosition() const { return transform.position; }

			bool sphereInFrustum(glm::vec3 pos, float radius) const;

			float getFov() const { return fov; }
			float getAspect() const { return aspect; }
			float getNear() const { return near; }
			float getFar() const { return far; }

			maths::transform getTransform() const { return transform; }

			enum FrustumPoints {
				NEAR_TOP_LEFT,
				NEAR_TOP_RIGHT,
				NEAR_BOTTOM_LEFT,
				NEAR_BOTTOM_RIGHT,
				FAR_TOP_LEFT,
				FAR_TOP_RIGHT,
				FAR_BOTTOM_LEFT,
				FAR_BOTTOM_RIGHT,
			};
		};
	}
}

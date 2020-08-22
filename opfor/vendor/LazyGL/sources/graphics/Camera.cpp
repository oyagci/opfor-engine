//
// Created by Marc on 01/06/2018.
//

#include "Camera.hpp"
#include "inputs/Input.hpp"
#include "inputs/Keyboard.hpp"

using namespace lazy;
using namespace inputs;

namespace lazy
{
	namespace graphics
	{
		Camera::Camera(const Display &display, const maths::transform &trs)
			: display(display), transform(trs), fov(80.0), aspect(display.getAspect()), projection(glm::mat4(1)),
			  frustumPlanes{}
		{
			updateFrustum();
		}

		Camera::~Camera()
		{

		}

		void Camera::input(float moveSpeed, float mouseSpeed, std::array<GLuint, 6> keys)
		{
			float rotX = input::getMouse().getVelocity().x * -mouseSpeed;
			float rotY = input::getMouse().getVelocity().y * -mouseSpeed;
			
			if (rotX > 90) rotX = 90;
			if (rotX < -90) rotX = -90;

			transform.rotate(rotX, glm::vec3(0, 1, 0));
			transform.rotate(rotY, transform.right());

			if (keys[0] && input::getKeyboard().getKey(keys[0]))
				transform.translate(transform.forward() * moveSpeed);
			if (keys[1] && input::getKeyboard().getKey(keys[1]))
				transform.translate(transform.back() * moveSpeed);
			if (keys[2] && input::getKeyboard().getKey(keys[2]))
				transform.translate(transform.left() * moveSpeed);
			if (keys[3] && input::getKeyboard().getKey(keys[3]))
				transform.translate(transform.right() * moveSpeed);
			if (keys[4] && input::getKeyboard().getKey(keys[4]))
				transform.translate(transform.bottom() * moveSpeed);
			if (keys[5] && input::getKeyboard().getKey(keys[5]))
				transform.translate(transform.top() * moveSpeed);
		}

		void Camera::update()
		{
			if (display.hasResized())
				this->updateProjection();
			updateFrustum();
		}

		void Camera::updateFrustum()
		{
			glm::vec3 dir = transform.forward();
			glm::vec3 up = transform.top();
			glm::vec3 right = transform.right();

			float hFar = 2 * std::tan(fov / 2.0f) * far;
			float wFar = hFar * aspect;

			float hNear = 2 * std::tan(fov / 2.0f) * near;
			float wNear = hNear * aspect;

			glm::vec3 farP = transform.position + dir * far;

			// Far Plane vertex positions
			glm::vec3 ftl = farP + (up * (hFar / 2.0f)) - (right * (wFar / 2.0f));
			glm::vec3 ftr = farP + (up * (hFar / 2.0f)) + (right * (wFar / 2.0f));
			glm::vec3 fbl = farP - (up * (hFar / 2.0f)) - (right * (wFar / 2.0f));
			glm::vec3 fbr = farP - (up * (hFar / 2.0f)) + (right * (wFar / 2.0f));

			glm::vec3 nearP = transform.position + dir * near;

			// Near Plane vertex positions
			glm::vec3 ntl = nearP + (up * (hNear / 2.0f)) - (right * (wNear / 2.0f));
			glm::vec3 ntr = nearP + (up * (hNear / 2.0f)) + (right * (wNear / 2.0f));
			glm::vec3 nbl = nearP - (up * (hNear / 2.0f)) - (right * (wNear / 2.0f));
			glm::vec3 nbr = nearP - (up * (hNear / 2.0f)) + (right * (wNear / 2.0f));

			auto calcPlaneNormal = [] (glm::vec3 a, glm::vec3 b, glm::vec3 c) -> std::pair<glm::vec3, float> {
				glm::vec3 v = b - a;
				glm::vec3 u = c - a;
				glm::vec3 n = glm::normalize(glm::cross(v, u));

				float d = glm::dot(-n, a);

				return std::make_pair(n, d);
			};

			frustumPlanes[TOP] = calcPlaneNormal(ntr, ntl, ftl);
			frustumPlanes[BOTTOM] = calcPlaneNormal(nbl, nbr, fbr);
			frustumPlanes[LEFT] = calcPlaneNormal(ntl, nbl, fbl);
			frustumPlanes[RIGHT] = calcPlaneNormal(nbr, ntr, fbr);
			frustumPlanes[NEAR] = calcPlaneNormal(ntl, ntr, nbr);
			frustumPlanes[FAR] = calcPlaneNormal(ftr, ftl, fbl);
		}

		void Camera::setProjection(float fov, float near, float far)
		{
			this->fov = fov;
			this->near = near;
			this->far = far;
			this->updateProjection();
		}

		void Camera::updateProjection()
		{
			this->projection = glm::perspective(fov, display.getAspect(), near, far);
		}

		void Camera::setPosition(glm::vec3 pos)
		{
			this->transform.position = std::move(pos);
		}

		bool Camera::sphereInFrustum(glm::vec3 position, float radius) const
		{
			for (size_t i = 0; i < 6; i++) {

				glm::vec3 const &plane = frustumPlanes[i].first;
				float const d = frustumPlanes[i].second;
				float dist = glm::dot(plane, position) + d;

				if (dist < -radius) {
					return false;
				}
			}
			return true;
		}
	}
}

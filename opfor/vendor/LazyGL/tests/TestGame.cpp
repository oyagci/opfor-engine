//
// Created by mploux on 20/12/18.
//

#include "lazy.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;

int main()
{
	Display display("LazyGL", 1280, 720);
	display.enableCap(GL_DEPTH_TEST);
	display.enableCap(GL_CULL_FACE);

	Mesh mesh;
	mesh.addPosition(glm::vec3(0, 0, 0))
			.addPosition(glm::vec3(1, 0, 0))
			.addPosition(glm::vec3(0, 1, 0))
			.addTriangle(glm::u32vec3(0, 1, 2));
	mesh.build();

	Shader shader;
	shader.addVertexShader("../../shaders/main_v.glsl")
			.addFragmentShader("../../shaders/main_f.glsl");
	shader.link();

	Camera camera(display, (maths::transform){glm::vec3(0, 0, 5), glm::quat(), glm::vec3(1), nullptr});
	camera.setProjection(70.0f, 0.1f, 1000.0f);

	std::cout << "Random: " << lazy::maths::random() << std::endl;

	float angle = 0;
	int frames = 0;
	double startTime = glfwGetTime();
	while (!display.isClosed())
	{
		display.update();
		camera.update();

		double currentTime = glfwGetTime();
		if (currentTime - startTime >= 1.0 / 60.0)
		{
			display.updateInputs();

			if (input::getKeyboard().getKeyDown(GLFW_KEY_A))
				std::cout << "A DOWN" << std::endl;
			if (input::getKeyboard().getKey(GLFW_KEY_A))
				std::cout << "A" << std::endl;
			if (input::getKeyboard().getKeyUp(GLFW_KEY_A))
				std::cout << "A UP" << std::endl;
			
			if (display.isFocused())
				camera.input(0.1f, 0.001f, {
					GLFW_KEY_W,
					GLFW_KEY_S,
					GLFW_KEY_A,
					GLFW_KEY_D,
					GLFW_KEY_LEFT_SHIFT,
					GLFW_KEY_SPACE
				});

			startTime = currentTime;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.bind();
		shader.setUniform4x4f("projectionMatrix", camera.getProjectionMatrix());
		shader.setUniform4x4f("viewMatrix", camera.getViewMatrix());
		shader.setUniform4x4f("viewProjectionMatrix", camera.getViewProjectionMatrix());

		// angle += 0.0001f;
		shader.setUniform4x4f("modelMatrix", glm::rotate(angle, glm::vec3(1, 1, 0)));
		mesh.draw();
	}

	return EXIT_SUCCESS;
}

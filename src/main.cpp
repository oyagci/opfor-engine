#include "Engine.hpp"
#include "engine/assimp/Model.hpp"
#include "systems/CameraMovementSystem.hpp"
#include "systems/MeshRendererSystem.hpp"
#include "systems/SkyboxRendererSystem.hpp"
#include "systems/ImguiSystem.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "stb_image.h"
#include "lazy.hpp"

using namespace engine;

SkyboxComponent initSkybox()
{
	SkyboxComponent s;

	std::array<std::string, 6> paths = {
		"./img/right.jpg",
		"./img/left.jpg",
		"./img/top.jpg",
		"./img/bottom.jpg",
		"./img/front.jpg",
		"./img/back.jpg",
	};

	glGenTextures(1, &s.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, s.texture);

	stbi_set_flip_vertically_on_load(false);
	for (size_t i = 0; i < 6; i++) {
		int w, h, nchannel;
		unsigned char *data = stbi_load(paths[i].c_str(), &w, &h, &nchannel, 0);
		if (!data) {
			std::cerr << "WARNING::CUBEMAP::INIT::IMAGE_NOT_FOUND " << paths[i] << std::endl;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	GLfloat verts[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &s.vao);
	glGenBuffers(1, &s.vbo);

	glBindVertexArray(s.vao);
	glBindBuffer(GL_ARRAY_BUFFER, s.vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);

	glBindVertexArray(0);

	return s;
}

int main()
{
	auto &engine = Engine::Instance();
	engine.CreateComponentSystem<MeshRendererSystem>();
	engine.CreateComponentSystem<CameraMovementSystem>();
	engine.CreateComponentSystem<SkyboxRendererSystem>();
	engine.CreateComponentSystem<ImguiSystem>();

	auto player = engine.CreateEntity<PlayerCameraComponent, TransformComponent>();

	PlayerCameraComponent p;
		p.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 10000.0f);
		p.model = glm::mat4(1.0f);
		p.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		p.viewProjection = p.projection * p.view;
	player->SetComponentData<PlayerCameraComponent>(p);

	TransformComponent t;
		t.direction = glm::vec3(0.0f, 0.0f, 1.0f);
		t.position = glm::vec3(0.0f, 180.0f, -10.0f);
	player->SetComponentData<TransformComponent>(t);

	using MeshEntity = ecs::IEntity<MeshComponent>;

	auto sponza = assimp::Model("./Sponza/sponza.obj");
	std::vector<MeshEntity*> sponzaMeshes;
	{
		std::vector<assimp::Mesh> &meshes = sponza.getMeshes();

		for (auto &m : meshes) {
			MeshEntity *b = engine.CreateEntity<MeshComponent>();

			MeshComponent comp;
			comp.textures = std::vector<std::string>(m.getTextures());
			comp.mesh = std::move(m);
			b->SetComponentData<MeshComponent>(std::move(comp));

			sponzaMeshes.push_back(b);
		}
	}

	auto skybox = engine.CreateEntity<SkyboxComponent>();
	SkyboxComponent s = initSkybox();
	skybox->SetComponentData<SkyboxComponent>(s);

	auto display = engine.CreateEntity<DisplayComponent>();
	DisplayComponent d;
	d.display = engine.GetDisplay();
	display->SetComponentData<DisplayComponent>(d);

	return engine::Engine::Instance().Run();
}

#include "Engine.hpp"
#include "engine/assimp/Model.hpp"
#include "systems/CameraMovementSystem.hpp"
#include "systems/MeshRendererSystem.hpp"
#include "systems/SkyboxRendererSystem.hpp"
#include "systems/ImguiSystem.hpp"
#include "systems/BatchRendererSystem.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/BatchComponent.hpp"
#include "stb_image.h"
#include "lazy.hpp"
#include "ShaderManager.hpp"
#include <fmt/format.h>
#include <utility>
#include <string>
#include "Batch.hpp"
#include "Logger.hpp"

using namespace engine;

MeshComponent initSkybox()
{
	MeshComponent meshComponent;
	engine::Mesh mesh;
	GLuint textureId;

	const std::array<std::string, 6> paths = {
		"./img/right.jpg",
		"./img/left.jpg",
		"./img/top.jpg",
		"./img/bottom.jpg",
		"./img/front.jpg",
		"./img/back.jpg",
	};

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	stbi_set_flip_vertically_on_load(false);
	for (size_t i = 0; i < 6; i++) {
		int w, h, nchannel;
		unsigned char *data = stbi_load(paths[i].c_str(), &w, &h, &nchannel, 0);
		if (!data) {
			fmt::print(stderr, "WARNING::CUBEMAP::INIT::IMAGE_NOT_FOUND {}\n", paths[i]);
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

	Texture texture(textureId, "skybox-cubemap", 0, 0, 0, GL_TEXTURE_CUBE_MAP);
	TextureManager::instance().add("skybox-cubemap", std::move(texture));

	const glm::vec3 verts[] = {
		glm::vec3(-1.0, -1.0,  1.0),
		glm::vec3( 1.0, -1.0,  1.0),
		glm::vec3( 1.0,  1.0,  1.0),
		glm::vec3(-1.0,  1.0,  1.0),

		glm::vec3(-1.0, -1.0, -1.0),
		glm::vec3( 1.0, -1.0, -1.0),
		glm::vec3( 1.0,  1.0, -1.0),
		glm::vec3(-1.0,  1.0, -1.0),
	};

	const glm::uvec3 indices[] = {
		// front
		glm::uvec3(0, 3, 1),
		glm::uvec3(1, 3, 2),
		// right
		glm::uvec3(1, 2, 5),
		glm::uvec3(2, 6, 5),
		// back
		glm::uvec3(4, 5, 6),
		glm::uvec3(4, 6, 7),
		// left
		glm::uvec3(0, 4, 7),
		glm::uvec3(0, 7, 3),
		// bottom
		glm::uvec3(0, 1, 4),
		glm::uvec3(1, 5, 4),
		// top
		glm::uvec3(3, 7, 2),
		glm::uvec3(2, 7, 6)
	};

	mesh.addTexture("skybox-cubemap", engine::Mesh::TextureType::TT_Diffuse);
	for (const auto &v : verts) { mesh.addPosition(v); }
	for (const auto &i : indices) { mesh.addTriangle(i); }
	mesh.build();

	meshComponent.Id = Engine::Instance().AddMesh(std::move(mesh));

	return meshComponent;
}

std::vector<unsigned int> LoadMesh(std::string const &path, std::string const &name)
{
	// Load Assimp Model
	assimp::Model model(path);
	std::vector<engine::Mesh> meshes;

	// Convert assimp::Mesh to engine::Mesh
	for (auto const &assimp : model.getMeshes()) {
		engine::Mesh mesh;

		assert(assimp.Positions.size() % 3 == 0);
		assert(assimp.Normals.size() % 3 == 0);
		assert(assimp.Tangents.size() % 3 == 0);
		for (size_t i = 0; i < assimp.Positions.size(); i += 3) {
			glm::vec3 pos{ assimp.Positions[0 + i], assimp.Positions[1 + i],   assimp.Positions[2 + i] };
			glm::vec3 norm{  assimp.Normals[0 + i],   assimp.Normals[1 + i],     assimp.Normals[2 + i] };
			glm::vec3 tan{  assimp.Tangents[0 + i],  assimp.Tangents[1 + i],    assimp.Tangents[2 + i] };

			mesh.addPosition(pos);
			mesh.addNormal(norm);
			mesh.addTangent(tan);
		}

		assert(assimp.TexCoords.size() % 2 == 0);
		for (size_t i = 0; i < assimp.TexCoords.size(); i += 2) {
			glm::vec2 tex{ assimp.TexCoords[0 + i], assimp.TexCoords[1 + i] };
			mesh.addUv(tex);
		}

		for (size_t i = 0; i < assimp.Indices.size(); i++) {
			mesh.addIndex(assimp.Indices[i]);
		}

		mesh.build();
		meshes.push_back(std::move(mesh));
	}

	// Load Textures for Material
	for (auto const &[type, texture] : model.getTextures()) {
		for (auto const &t : texture) {
			Logger::Verbose("Loading texture {} into memory\n", t.name);
			TextureManager::instance().createTexture(t.name, t.path, {
				{ GL_TEXTURE_MAG_FILTER, GL_LINEAR },
				{ GL_TEXTURE_MIN_FILTER, GL_LINEAR },
				{ GL_TEXTURE_WRAP_S, GL_REPEAT },
				{ GL_TEXTURE_WRAP_T, GL_REPEAT },
			}, GL_TEXTURE_2D);
		}
	}

	// Create Material for Meshes
	std::vector<Material> materials{}; 
	std::vector<std::string> materialNames{};
	size_t matIndex = 0;
	size_t i = 0;
	for (auto &mesh : model.getMeshes()) {
		Material mat{};

		mat.shininess = 32.0f;
		if (mesh.DiffuseMaps.size() > 0) {
			mat.diffuse = TextureManager::instance().get(mesh.DiffuseMaps[0]);
		}
		if (mesh.NormalMaps.size() > 0) {
			mat.normal = TextureManager::instance().get(mesh.NormalMaps[0]);
		}
		if (mesh.SpecularMaps.size() > 0) {
			mat.specular = TextureManager::instance().get(mesh.SpecularMaps[0]);
		}
		
		auto checkMaterialExists = [&] (std::vector<Material> &materials, Material &rhs) -> std::optional<size_t> {
			std::optional<size_t> matIndex;

			for (size_t i = 0; i < materials.size(); i++) {
				auto &mat = materials[i];
				if (rhs.diffuse == mat.diffuse &&
					rhs.normal == mat.normal &&
					rhs.specular == mat.specular) {
					matIndex = i;
				}
			}

			return matIndex;
		};

		auto existingIndex = checkMaterialExists(materials, mat);
		if (existingIndex.has_value()) {
			meshes[i].SetMaterial(materials[existingIndex.value()].name);
			Logger::Verbose("Set material {} {}", materials[existingIndex.value()].name, mat);
		}
		else {
			std::string materialName(name + std::to_string(matIndex++));
			mat.name = materialName;
			materials.push_back(mat);
			engine::Engine::Instance().AddMaterial(materialName, mat);
			materialNames.push_back(materialName);
			meshes[i].SetMaterial(materialName);
			Logger::Verbose("Create material {} {}", materialName, mat);
		}

		i++;
	}

	std::vector<unsigned int> meshIds;
	for (auto &m : meshes) {
		meshIds.push_back(engine::Engine::Instance().AddMesh(std::move(m)));
	}

	return meshIds;
}

int main()
{

	Logger::Verbose("Hello World\n");
	auto &engine = Engine::Instance();
	engine.CreateComponentSystem<CameraMovementSystem>();
	engine.CreateComponentSystem<SkyboxRendererSystem>();
	engine.CreateComponentSystem<MeshRendererSystem>();
	engine.CreateComponentSystem<ImguiSystem>();

	TextureManager::instance().createTexture("prototype_tile_8", "./img/prototype_tile_8.png", {
		{ GL_TEXTURE_MAG_FILTER, GL_LINEAR },
		{ GL_TEXTURE_MIN_FILTER, GL_LINEAR },
		{ GL_TEXTURE_WRAP_R, GL_REPEAT },
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
	}, GL_TEXTURE_2D);

	TextureManager::instance().createTexture("default_normal", "./img/default_normal.png", {
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		{ GL_TEXTURE_WRAP_R, GL_REPEAT },
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
	}, GL_TEXTURE_2D);

	auto player = engine.CreateEntity<PlayerCameraComponent, TransformComponent>();

	PlayerCameraComponent p;
		p.projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 10000.0f);
		p.model = glm::mat4(1.0f);
		p.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		p.viewProjection = p.projection * p.view;
	player->Set(p);

	TransformComponent t;
		t.direction = glm::vec3(0.0f, 0.0f, 1.0f);
		t.position = glm::vec3(0.0f, 180.0f, -5.0f);
	player->Set(t);

	auto [ shaderId, meshShader ] = ShaderManager::instance().Create();
	meshShader.addVertexShader("./shaders/basic.vs.glsl")
		.addFragmentShader("./shaders/basic.fs.glsl")
		.link();

	meshShader.bind();
	meshShader.setUniform1i("material.diffuse", 0);
	meshShader.setUniform1i("material.specular", 1);
	meshShader.setUniform1i("material.normal", 2);
	meshShader.unbind();

	auto [ skyboxShaderId, skyboxShader ] = ShaderManager::instance().Create();
	skyboxShader.addVertexShader("./shaders/cubemap.vs.glsl")
		.addFragmentShader("./shaders/cubemap.fs.glsl")
		.link();
	skyboxShader.bind();
	skyboxShader.setUniform1i("cubemap", 0);
	skyboxShader.unbind();

	auto dvaMeshIds = LoadMesh("models/dva/0.obj", "dva");
	{
		for (auto const &meshId : dvaMeshIds) {
			auto dvaEnt = engine.CreateEntity<MeshComponent, TransformComponent>();
			MeshComponent dvaMesh{};
				dvaMesh.Id = meshId;
				dvaMesh.Shader = shaderId;
			dvaEnt->Set(dvaMesh);

			TransformComponent t{};
				t.scale = { 100.0f, 100.0f, 100.0f };
			dvaEnt->Set(t);
		}
	}

	auto sponMeshIds = LoadMesh("models/Sponza/sponza.obj", "sponza");
	for (auto const &meshId : sponMeshIds) {
		auto spon = engine.CreateEntity<MeshComponent, TransformComponent>();
		MeshComponent dvaMesh{};
			dvaMesh.Id = meshId;
			dvaMesh.Shader = shaderId;
		spon->Set(dvaMesh);

		TransformComponent t{};
			t.scale = { 1.0f, 1.0f, 1.0f };
		spon->Set(t);
	}

//	// Create a batch for the meshes
//	auto batch = std::make_unique<Batch>();
//	int i = 0;
//	for (auto const &m : dvaModel.getMeshes()) {
//		batch->AddMesh(
//			m.Positions,
//			m.Normals,
//			m.TexCoords,
//			m.Tangents,
//			materialNames[i++],
//			m.Indices);
//	}
//	batch->Build();
	
//	unsigned int batchId = engine.AddBatch(std::move(batch));
//	auto *batchEnt = engine.CreateEntity<MeshComponent, TransformComponent>();
//	{
//		TransformComponent t;
//			t.scale = glm::vec3(100.0f, 100.0f, 100.0f);
//			t.position = glm::vec3(0.0f, 0.0f, 0.0f);
//		MeshComponent mesh;
//			mesh.Id = batchId;
//			mesh.Shader = shaderId;
//		batchEnt->Set(t);
//		batchEnt->Set(mesh);
//	}

//	auto sponza = engine.LoadMeshes("models/Sponza/sponza.obj");
//	{
//		for (auto &m : sponza) {
//			auto *b = engine.CreateEntity<MeshComponent,
//											    TransformComponent>();
//
//			MeshComponent mesh;
//			mesh.Id = m;
//			mesh.Shader = shaderId;
//			b->Set(mesh);
//		}
//	}

	auto skybox = engine.CreateEntity<MeshComponent, TransformComponent, SkyboxComponent>();

	MeshComponent s = initSkybox();
		s.Shader = skyboxShaderId;
	skybox->Set(s);

	auto display = engine.CreateEntity<DisplayComponent>();
	DisplayComponent d;
	d.display = engine.GetDisplay();
	display->Set(d);

	auto pointLight = engine.CreateEntity<PointLightComponent, TransformComponent, SelectedComponent>();
	PointLightComponent pl;
	pl.Color = glm::vec3(1.0f, 1.0f, 0.8f);
	pl.Dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
	pointLight->Set(pl);

	TransformComponent pt;
	pt.position = glm::vec3(-500.0f, 100.0f, 0.0f);
	pointLight->Set(pt);

//	auto pointLight2 = engine.CreateEntity<PointLightComponent, TransformComponent>();
//	pl.Color = glm::vec3(1.0f, 1.0f, 0.8f);
//	pl.Dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
//	pointLight2->Set(pl);
//
//	pt.position = glm::vec3(500.0f, 100.0f, 0.0f);
//	pointLight2->Set(pt);

	return engine::Engine::Instance().Run();
}

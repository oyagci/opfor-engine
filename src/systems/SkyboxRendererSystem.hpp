#pragma once

#include "ecs/System.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "lazy.hpp"
#include "ShaderManager.hpp"
#include "components/MeshComponent.hpp"
#include "Engine.hpp"
#include "TextureManager.hpp"
#include <fmt/format.h>
#include "engine/renderer/Shader.hpp"
#include "stb_image.h"
#include "engine/renderer/Renderer.hpp"
#include "engine/renderer/Texture.hpp"

using namespace std::placeholders;

class SkyboxRendererSystem : public ecs::ComponentSystem
{
private:
	opfor::UniquePtr<opfor::Shader> _shader;
	opfor::SharedPtr<opfor::Texture> _SkyboxCubemap;
	opfor::Mesh _SkyboxCubeMesh;

	void InitSkybox()
	{
		GLuint textureId;

		const std::array<std::string, 6> paths = {
			"./img/right.jpg",
			"./img/left.jpg",
			"./img/top.jpg",
			"./img/bottom.jpg",
			"./img/front.jpg",
			"./img/back.jpg",
		};

		opfor::TextureParameterList params = {
			{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Nearest },
			{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Nearest },
			{ opfor::TextureParameterType::WrapR,         opfor::TextureParameterValue::ClampToEdge },
			{ opfor::TextureParameterType::WrapS,         opfor::TextureParameterValue::ClampToEdge },
			{ opfor::TextureParameterType::WrapT,         opfor::TextureParameterValue::ClampToEdge },
		};
		_SkyboxCubemap = opfor::Texture::Create();

		_SkyboxCubemap->SetTextureType(opfor::TextureType::TexCubemap);
		_SkyboxCubemap->SetTextureParameters(params);
		_SkyboxCubemap->SetInputFormat(opfor::DataFormat::RGB);
		_SkyboxCubemap->SetOutputFormat(opfor::DataFormat::RGB);
		_SkyboxCubemap->SetDataType(opfor::DataType::UnsignedByte);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

		_SkyboxCubemap->Bind(opfor::TextureUnit::Texture0);

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

		_SkyboxCubemap->Unbind();

		TextureManager::Get().Add("skybox-cubemap", _SkyboxCubemap);

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

		_SkyboxCubeMesh.addTexture("skybox-cubemap", opfor::Mesh::TextureType::TT_Diffuse);
		for (const auto &v : verts) { _SkyboxCubeMesh.addPosition(v); }
		for (const auto &i : indices) { _SkyboxCubeMesh.addTriangle(i); }
		_SkyboxCubeMesh.build();
	}

public:
	SkyboxRendererSystem()
	{
		_shader = opfor::Shader::Create();
		_shader->AddVertexShader("shaders/cubemap.vs.glsl");
		_shader->AddFragmentShader("shaders/cubemap.fs.glsl");
		_shader->Link();

		InitSkybox();
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto camera = GetEntities<PlayerCameraComponent>();
		auto skybox = GetEntities<SkyboxComponent>();

		if (skybox.size() == 0) { return ; }
		if (camera.size() == 0) { return ; }

		auto cameraData = camera[0]->Get<PlayerCameraComponent>();

		_shader->Bind();
		_shader->SetUniform("viewMatrix", glm::mat4(glm::mat3(cameraData.view)));
		_shader->SetUniform("projectionMatrix", cameraData.projection);

		glDepthMask(GL_FALSE);
		TextureManager::Get().Get("skybox-cubemap")->Bind(opfor::TextureUnit::Texture0);
		_SkyboxCubeMesh.Draw();
		TextureManager::Get().Get("skybox-cubemap")->Unbind();
		glDepthMask(GL_TRUE);

		_shader->Unbind();
	}
};

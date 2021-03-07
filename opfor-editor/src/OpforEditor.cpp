#include "SpheresRotateSystem.hpp"
#include "components/ModelComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/TransformComponent.hpp"
#include "opfor.hpp"
#include "opfor/renderer/Model.hpp"
#include "opfor/renderer/ShaderManager.hpp"

#include "editor/EditorInspector.hpp"
#include "editor/EditorLog.hpp"
#include "editor/EditorMenuBar.hpp"
#include "editor/EditorSceneHierarchy.hpp"
#include "editor/EditorViewport.hpp"

struct Container : ecs::IComponent
{
};

class ContainerSystem : public ecs::ComponentSystem
{
  private:
    float totalTime = 0.0f;

  public:
    void OnUpdate(float deltaTime) override
    {
        totalTime += deltaTime;

        auto containers = GetEntities<TransformComponent, Container>();

        for (auto c : containers)
        {
            auto &t = c->Get<TransformComponent>();

            t.position.x = sinf(totalTime) * 1000;
        }
    }
};

class OpforEditor : public opfor::Application
{
  private:
    void LoadModel(std::string const &filename, ModelComponent &modelComponent)
    {
        opfor::Model model{};

        model.LoadFromGLTF(filename);

        auto const &meshes = model.GetMeshes();
        if (!meshes.empty())
        {
            modelComponent.meshes.reserve(meshes.size());
            modelComponent.meshes.insert(modelComponent.meshes.end(), meshes.begin(), meshes.end());
        }
    }

  public:
    OpforEditor() : opfor::Application()
    {
        // opfor::Application::Get().LoadLevel("assets/levels/pbr.level");

        ImGuiLayer::Get().OpenWindow<EditorViewport>();
        ImGuiLayer::Get().OpenWindow<EditorMenuBar>();
        ImGuiLayer::Get().OpenWindow<EditorLog>();
        ImGuiLayer::Get().OpenWindow<EditorSceneHierarchy>();
        ImGuiLayer::Get().OpenWindow<EditorInspector>();

        CreateComponentSystem<SpheresRotateSystem>();
        CreateComponentSystem<ContainerSystem>();

        auto *skyBox = CreateEntity<SkyboxComponent>();
        skyBox->SetName("SkyBox");

        auto [spheresShaderID, spheresShader] = ShaderManager::Get().Create("shaders/basic.glsl");

        auto *spheres = CreateEntity<SphereComponent, TransformComponent, ModelComponent>();
        auto &sphereModel = spheres->Get<ModelComponent>();
        LoadModel("./assets/models/metal-sphere/metal-sphere.gltf", sphereModel);
        sphereModel.shader = spheresShaderID;
        spheresShader.Bind();
        spheresShader.SetUniform("material.albedo", 0);
        spheresShader.SetUniform("material.metallicRoughness", 1);
        spheresShader.SetUniform("material.normal", 2);
        spheresShader.SetUniform("material.metallicFactor", 1.0f);
        spheresShader.SetUniform("material.roughnessFactor", 1.0f);
        spheresShader.Unbind();
        spheres->SetName("Spheres");

        auto *container = CreateEntity<TransformComponent, Container>();
        container->SetName("Container");

        auto &containerTransform = container->Get<TransformComponent>();
        spheres->Get<TransformComponent>().parent = containerTransform;

        auto *child = CreateEntity<TransformComponent>();
        child->SetName("Child1");
        child->Get<TransformComponent>().parent = containerTransform;

        auto *childchild = CreateEntity<TransformComponent>();
        childchild->SetName("ChildChild1");
        childchild->Get<TransformComponent>().parent = child->Get<TransformComponent>();

        auto *child2 = CreateEntity<TransformComponent>();
        child2->SetName("Child2");
        child2->Get<TransformComponent>().parent = containerTransform;
    }
};

namespace opfor
{

UniquePtr<Application> CreateApplication()
{
    return MakeUnique<OpforEditor>();
}

} // namespace opfor

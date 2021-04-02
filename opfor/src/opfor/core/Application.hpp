#pragma once

#include <memory>
#include <opfor/core/Logger.hpp>
#include <opfor/core/Window.hpp>
#include <opfor/core/base.hpp>
#include <opfor/core/events/EngineEvents.hpp>
#include <opfor/ecs/ecs.hpp>
#include <opfor/layers/LayerStack.hpp>
#include <opfor/renderer/Batch.hpp>
#include <opfor/renderer/Context.hpp>
#include <opfor/renderer/Mesh.hpp>
#include <opfor/renderer/PerspectiveCameraController.hpp>
#include <opfor/renderer/SceneRenderer.hpp>
#include <opfor/renderer/Viewport.hpp>
#include <unordered_map>

namespace ecs
{
class SystemManager;
class EntityManager;
} // namespace ecs

struct ModelComponent;

namespace opfor
{
class ImGuiWrapperLayer;

class Model;

class Application
{
  private:
    static inline Application *_Instance;

    UniquePtr<IWindow> _window;
    UniquePtr<IRendererContext> _context;

    ecs::ECSEngine _ecs;

    ecs::EntityManager *_entityManager;
    ecs::SystemManager *_systemManager;

    UniquePtr<PerspectiveCameraController> _camera;

    UnorderedMap<unsigned int, UniquePtr<Model>> _models;
    UnorderedMap<unsigned int, UniquePtr<IDrawable>> _meshes;
    UnorderedMap<unsigned int, UniquePtr<Batch>> _batches;

    using MaterialContainer = std::pair<unsigned int, Material>;

    UnorderedMap<String, MaterialContainer> _materials;
    UnorderedMap<String, PbrMaterial> _pbrMaterials;

    Viewport *_viewport;

    static unsigned int _nextId;
    static unsigned int _nextMaterialId;

    enum class PlayState
    {
        Stopped,
        Playing,
        Paused,
    };
    PlayState _isPlaying;

    LayerStack _LayerStack;

    SceneRenderer *_SceneRenderer;
    ImGuiWrapperLayer *_ImGuiLayer;

    void InitViewport();

  public:
    // Editor
    void OnRebuildModel(ModelComponent &model);

    auto IsPlaying() const
    {
        return _isPlaying == PlayState::Playing;
    }

  public:
    Application();
    ~Application();
    Application(Application const &) = delete;

    void operator=(Application const &) = delete;

    static inline Application &Get()
    {
        return *_Instance;
    }

    int Run();

    IWindow *GetWindow() const
    {
        return _window.get();
    }

    template <typename T, typename... ArgTypes> void CreateComponentSystem(ArgTypes... args)
    {
        static_assert(std::is_base_of<ecs::ComponentSystem, T>::value && !std::is_same<ecs::ComponentSystem, T>::value,
                      "T must be derived from ComponentSystem");

        _ecs.GetSystemManager()->InstantiateSystem<T>(std::forward(args)...);
    }

    template <typename... ArgTypes>[[nodiscard]] ecs::IEntity<ArgTypes...> *CreateEntity()
    {
        return _ecs.GetEntityManager()->CreateEntity<ArgTypes...>();
    }

    unsigned int AddMesh(Mesh mesh)
    {
        auto to_ptr = MakeUnique<Mesh>(std::move(mesh));
        _meshes[_nextId] = std::move(to_ptr);

        return _nextId++;
    }

    IDrawable *GetMesh(unsigned int id)
    {
        auto mesh = _meshes.find(id);

        if (mesh != _meshes.end())
        {
            return mesh->second.get();
        }

        return nullptr;
    }

    unsigned int AddBatch(UniquePtr<Batch> batch)
    {
        _meshes[_nextId] = std::move(batch);

        return _nextId++;
    }

    void AddPbrMaterial(PbrMaterial material)
    {
        if (_pbrMaterials.find(material.Name) != _pbrMaterials.end())
        {
            Logger::Warn("Material {} already exists\n", material.Name);
        }

        _pbrMaterials[material.Name] = material;
    }

    Optional<PbrMaterial const *> GetPbrMaterial(String const &name)
    {
        if (_pbrMaterials.find(name) != _pbrMaterials.end())
        {
            return std::make_optional(&_pbrMaterials[name]);
        }
        return std::nullopt;
    }

    unsigned int GetMaterialId(String const &name)
    {
        return _materials[name].first;
    }

    Vector<String> GetMaterialList() const
    {
        Vector<String> materials;

        materials.resize(_materials.size());
        std::transform(_materials.begin(), _materials.end(), materials.begin(), [](auto mat) { return mat.first; });
        return materials;
    }

    void Close() const
    {
        _window->Close();
    }

    unsigned int RegisterModel(Model model);
    Optional<Model const *> GetModel(unsigned int id) const;
    void RemoveModel(unsigned int id);

    auto GetEntity(uuids::uuid const &uuid) const
    {
        return _entityManager->GetEntity(uuid);
    }

    void DeleteEntity(uuids::uuid const &uuid) const
    {
        _entityManager->DeleteEntity(uuid);
    }

    auto GetViewport() const
    {
        return _viewport;
    }
    auto GetViewportTexture() const
    {
        return _viewport->GetRawHandle();
    }

    void OnEvent(Event &);
    bool OnWindowResize(WindowResizeEvent &e);
    bool OnWindowClose(WindowCloseEvent &e);
    bool OnViewportResize(ViewportResizeEvent &e);

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *overlay);
    void PopLayer(Layer *layer);
    void PopOverlay(Layer *overlay);

    template <typename... ArgTypes> Vector<ecs::IEntity<ArgTypes...> *> GetEntities()
    {
        return _ecs.GetEntityManager()->GetEntities<ArgTypes...>();
    }

    Vector<ecs::IEntityBase *> GetAllEntities()
    {
        return _ecs.GetEntityManager()->GetAllEntities();
    }

    void RenderImgui();

    auto &GetCameraController() const
    {
        return *_camera;
    }
};

UniquePtr<Application> CreateApplication();

} // namespace opfor

#pragma once

#include "EngineObject.hpp"
#include "ILevel.hpp"
#include "ecs/ecs.hpp"
#include "opfor/core/Action.hpp"
#include "opfor/core/Logger.hpp"
#include "opfor/core/Window.hpp"
#include "opfor/core/base.hpp"
#include "opfor/core/events/EngineEvents.hpp"
#include "opfor/layers/ImGuiLayer.hpp"
#include "opfor/layers/LayerStack.hpp"
#include "opfor/renderer/Batch.hpp"
#include "opfor/renderer/Context.hpp"
#include "opfor/renderer/Mesh.hpp"
#include "opfor/renderer/PerspectiveCameraController.hpp"
#include "opfor/renderer/SceneRenderer.hpp"
#include "opfor/renderer/Viewport.hpp"
#include <fmt/format.h>
#include <memory>
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

    Vector<std::unique_ptr<EngineObject>> _engineObjects;

    std::unordered_map<unsigned int, std::unique_ptr<opfor::Model>> _models;
    std::unordered_map<unsigned int, std::unique_ptr<IDrawable>> _meshes;
    std::unordered_map<unsigned int, std::unique_ptr<Batch>> _batches;

    using MaterialContainer = std::pair<unsigned int, Material>;

    std::unordered_map<std::string, MaterialContainer> _materials;
    std::unordered_map<std::string, PbrMaterial> _pbrMaterials;

    opfor::Viewport *_viewport;

    static unsigned int _nextId;
    static unsigned int _nextMaterialId;

    enum class PlayState
    {
        Stopped,
        Playing,
        Paused,
    };
    PlayState _isPlaying;

    Callback<size_t> _selectItem;

    LayerStack _LayerStack;

    SceneRenderer *_SceneRenderer;
    ImGuiWrapperLayer *_ImGuiLayer;

    void InitViewport();

  public:
    /// Select an item in the editor
    Action<size_t> OnSelectItem;

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
    void Update();
    void UpdateObjects();
    void UpdateSubobjects(Vector<EngineObject *> subobjects);

    IWindow *GetWindow() const
    {
        return _window.get();
    }

    template <typename T, typename... ArgTypes> [[nodiscard]] T *CreateEngineObject(ArgTypes &&...args)
    {
        static_assert(std::is_base_of<EngineObject, T>::value && !std::is_same<EngineObject, T>::value,
                      "T must be derived from EngineObject");

        auto object = std::make_unique<T>(&_ecs, std::forward<ArgTypes>(args)...);
        auto ret = object.get();

        _engineObjects.push_back(std::move(object));

        return ret;
    }

    template <typename T, typename... ArgTypes> void CreateComponentSystem(ArgTypes... args)
    {
        static_assert(std::is_base_of<ecs::ComponentSystem, T>::value && !std::is_same<ecs::ComponentSystem, T>::value,
                      "T must be derived from EngineObject");

        _ecs.GetSystemManager()->InstantiateSystem<T>(std::forward(args)...);
    }

    template <typename... ArgTypes> [[nodiscard]] ecs::IEntity<ArgTypes...> *CreateEntity()
    {
        return _ecs.GetEntityManager()->CreateEntity<ArgTypes...>();
    }

    unsigned int AddMesh(Mesh mesh)
    {
        auto to_ptr = std::make_unique<Mesh>(std::move(mesh));
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

    unsigned int AddBatch(std::unique_ptr<Batch> batch)
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

    std::optional<PbrMaterial const *> GetPbrMaterial(std::string const &name)
    {
        if (_pbrMaterials.find(name) != _pbrMaterials.end())
        {
            return std::make_optional(&_pbrMaterials[name]);
        }
        return std::nullopt;
    }

    unsigned int GetMaterialId(std::string const &name)
    {
        return _materials[name].first;
    }

    Vector<std::string> GetMaterialList() const
    {
        Vector<std::string> materials;

        materials.resize(_materials.size());
        std::transform(_materials.begin(), _materials.end(), materials.begin(), [](auto mat) { return mat.first; });
        return materials;
    }

    void Close() const
    {
        _window->Close();
    }

    unsigned int RegisterModel(opfor::Model model);
    Optional<opfor::Model const *> GetModel(unsigned int id) const;
    void RemoveModel(unsigned int id);

    auto GetEntity(unsigned int id) const
    {
        return _entityManager->GetEntity(id);
    }

    void DeleteEntity(unsigned int entityId) const
    {
        _entityManager->DeleteEntity(entityId);
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

    Vector<EngineObject *> GetSubobjects()
    {
        Vector<EngineObject *> subobjects;
        std::transform(_engineObjects.begin(), _engineObjects.end(), std::back_inserter(subobjects),
                       [](auto &item) { return item.get(); });
        return subobjects;
    }
};

UniquePtr<Application> CreateApplication();

} // namespace opfor

#include "Application.hpp"
#include <components/ModelComponent.hpp>
#include <opfor/Level.hpp>
#include <opfor/layers/ImGuiWrapperLayer.hpp>
#include <opfor/renderer/Context.hpp>
#include <opfor/renderer/Model.hpp>
#include <opfor/renderer/TextureManager.hpp>
#include <opfor/utils/Settings.hpp>
#include <opfor/utils/Time.hpp>
#include <platform/linux/LinuxInput.hpp>
#include <stb_image.h>
#ifdef OP4_PLATFORM_LINUX
#include <unistd.h>
#endif

namespace opfor
{

unsigned int Application::_nextId = 0;
unsigned int Application::_nextMaterialId = 0;

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
    _Instance = this;

    _window = IWindow::Create({"OPFOR - Untitled Project", 1920, 1080});
    _context = IRendererContext::Create(*_window);
    _context->Init();

    Input::Prime();

    _window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    _systemManager = _ecs.GetSystemManager();
    _entityManager = _ecs.GetEntityManager();

    _camera = MakeUnique<PerspectiveCameraController>();

    InitViewport();

    opfor::TextureParameterList texParams = {
        {opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Linear},
        {opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear},
        {opfor::TextureParameterType::WrapT, opfor::TextureParameterValue::Repeat},
        {opfor::TextureParameterType::WrapS, opfor::TextureParameterValue::Repeat},
    };

    auto prototype_tile = TextureManager::Get().Create2D("prototype_tile_8");
    auto img = opfor::ImageLoader::Load("./assets/img/prototype_tile_8.png");

    prototype_tile->SetDataType(opfor::DataType::UnsignedByte);
    prototype_tile->SetHasAlpha(img.nchannel == 4);
    prototype_tile->SetIsSRGB(true);
    prototype_tile->SetInputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
    prototype_tile->SetOutputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
    prototype_tile->SetSize(img.width, img.height);
    prototype_tile->SetTextureParameters(texParams);
    prototype_tile->SetData(img.data.get());
    prototype_tile->Build();

    auto default_normal = TextureManager::Get().Create2D("default_normal");
    img = opfor::ImageLoader::Load("./assets/img/default_normal.png");

    default_normal->SetDataType(opfor::DataType::UnsignedByte);
    default_normal->SetHasAlpha(img.nchannel == 4);
    default_normal->SetInputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
    default_normal->SetOutputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
    default_normal->SetSize(img.width, img.height);
    default_normal->SetTextureParameters(texParams);
    default_normal->SetData(img.data.get());
    default_normal->Build();

    _SceneRenderer = new SceneRenderer;
    _ImGuiLayer = new ImGuiWrapperLayer;

    PushOverlay(_ImGuiLayer);
    PushLayer(_viewport);
    PushLayer(_SceneRenderer);
}

/*
 * Needed because unique_ptr needs complete types
 * and there are some forward declared classes in the hpp file
 */
Application::~Application() = default;

void Application::InitViewport()
{
    _viewport = new Viewport(1920, 1080);
}

void Application::RenderImgui()
{
    _ImGuiLayer->OnImGuiRender();
}

int Application::Run()
{
    Settings::Get().load("config.ini");

    while (!_window->IsClosed())
    {
        float deltaTime = Time::Get().getDeltaTime();

        for (auto *layer : _LayerStack)
        {
            layer->OnUpdate(deltaTime);
        }
        _ecs.Update(deltaTime);
        _camera->Update(deltaTime);

        Renderer::BeginScene();

        Renderer::PushViewport({0, 0}, _viewport->GetSize());

        // TODO: Pick primary camera
        PerspectiveCamera &camera = opfor::Application::Get().GetCameraController().GetCamera();
        _SceneRenderer->RenderScene(camera);

        Application *app = this;
        Renderer::Submit([app]() { app->RenderImgui(); });

        Renderer::PopViewport();

        Renderer::EndScene();

        _context->SwapBuffers();

        Input::Flush();
    }

    return 0;
}

unsigned int Application::RegisterModel(Model model)
{
    auto modelPtr = std::make_unique<Model>(model);

    _models[_nextId] = std::move(modelPtr);
    return _nextId++;
}

void Application::RemoveModel(unsigned int id)
{
    auto model = _models.find(id);
    if (model != _models.end())
    {
        _models.erase(model);
    }
}

std::optional<Model const *> Application::GetModel(unsigned int id) const
{
    auto const model = _models.find(id);

    if (model != _models.end())
    {
        return std::make_optional(model->second.get());
    }

    return std::nullopt;
}

void Application::OnRebuildModel(ModelComponent &model)
{
    for (auto const &m : model.meshes)
    {
        RemoveModel(m);
    }
    model.meshes.clear();

    Model modelData{};
    modelData.LoadFromGLTF(model.path);

    auto const &meshes = modelData.GetMeshes();

    if (meshes.size() > 0)
    {
        model.meshes.reserve(meshes.size());
        model.meshes.insert(model.meshes.begin(), meshes.begin(), meshes.end());
    }
}

void Application::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.DispatchIf<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
    dispatcher.DispatchIf<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.DispatchIf<ViewportResizeEvent>(BIND_EVENT_FN(OnViewportResize));

    for (auto it = _LayerStack.rbegin(); it != _LayerStack.rend(); it++)
    {
        (*it)->OnEvent(e);
        if (e.Handled)
        {
            break;
        }
    }
}

bool Application::OnWindowResize(WindowResizeEvent &)
{
    return false;
}

bool Application::OnWindowClose(WindowCloseEvent &)
{
    return false;
}

bool Application::OnViewportResize(ViewportResizeEvent &)
{
    return false;
}

void Application::PushLayer(Layer *layer)
{
    _LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer *overlay)
{
    _LayerStack.PushOverlay(overlay);
    overlay->OnAttach();
}

void Application::PopLayer(Layer *layer)
{
    _LayerStack.PopLayer(layer);
    layer->OnDetach();
}

void Application::PopOverlay(Layer *overlay)
{
    _LayerStack.PopOverlay(overlay);
    overlay->OnDetach();
}

} // namespace opfor

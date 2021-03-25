#include "EditorMenuBar.hpp"

#include <nfd.h>

#include "EditorInspector.hpp"
#include "EditorSceneHierarchy.hpp"
#include <opfor/core/Application.hpp>
#include <opfor/core/LevelSerializer.hpp>

#ifdef OP4_PLATFORM_LINUX
#include <unistd.h>
#elif defined(OP4_PLATFORM_WINDOWS)
#include <direct.h>
#endif

opfor::UniquePtr<char[]> GetCwd()
{
#ifdef OP4_PLATFORM_LINUX
    auto buf = getcwd(nullptr, 0);

    auto ret = opfor::MakeUnique<char[]>(strlen(buf));
    strcpy(ret.get(), buf);

    return ret;
#elif defined(OP4_PLATFORM_WINDOWS)
    const DWORD bufLen = GetCurrentDirectory(0, nullptr);
    const auto buffer = opfor::MakeUnique<WCHAR[]>(bufLen);

    // std::wcstombs
    GetCurrentDirectory(bufLen, buffer.get());

    auto ret = opfor::MakeUnique<char[]>(bufLen);

    size_t pRetVal = 0;

    wcstombs_s(&pRetVal, ret.get(), bufLen, buffer.get(), bufLen);

    return ret;
#else
#error "Unsupported Platform!"
#endif
}

void EditorMenuBar::OnDrawGUI()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Close"))
            {
                opfor::Application::Get().Close();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("New Level"))
            {
            }
            if (ImGui::MenuItem("Open Level"))
            {
                char *outPath = nullptr;
                NFD_OpenDialog(nullptr, GetCwd().get(), &outPath);
                if (outPath)
                {
                    // opfor::Application::Get().LoadLevel(outPath);
                }
            }
            if (ImGui::MenuItem("Save Level"))
            {
                fmt::print("{}\n", opfor::LevelSerializer::Serialize());
            }
            if (ImGui::MenuItem("Save Level As..."))
            {
                char *outPath = nullptr;
                NFD_OpenDialog(nullptr, GetCwd().get(), &outPath);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Inspector"))
            {
                ImGuiLayer::Get().OpenWindow<EditorInspector>();
            }
            if (ImGui::MenuItem("Scene Hierarchy"))
            {
                ImGuiLayer::Get().OpenWindow<EditorSceneHierarchy>();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

workspace "opfor"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"]     = "opfor/vendor/glfw/include"
IncludeDir["GLAD"]     = "opfor/vendor/glad/include"
IncludeDir["fmt"]      = "opfor/vendor/fmt/include"
IncludeDir["ImGui"]    = "opfor/vendor/imgui"
IncludeDir["tinygltf"] = "opfor/vendor/tinygltf"
IncludeDir["glm"]      = "opfor/vendor/glm"
IncludeDir["stduuid"]  = "opfor/vendor/stduuid/include"
IncludeDir["NFD"]      = "opfor/vendor/nativefiledialog/src/include"
IncludeDir["ImGuizmo"] = "opfor/vendor/ImGuizmo"

-- "%{prj.name}/thirdparty",
-- "%{prj.name}/thirdparty/ImGuizmo",
-- "%{prj.name}/thirdparty/nativefiledialog/src/include",
-- "%{prj.name}/thirdparty/stduuid/include",
-- "%{prj.name}/thirdparty/glm",
-- "subprojects/glfw/include",
-- "subprojects/json/include",
-- "subprojects/LazyGL/includes",
-- "subprojects/fmt/include",

include "opfor/vendor/glfw"
include "opfor/vendor/glad"
include "opfor/vendor/fmt"
include "opfor/vendor/imgui"
include "opfor/vendor/ImGuizmo"
include "opfor/vendor/nativefiledialog"

project "opfor"
    location "opfor"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/stb_image.cpp",
        "%{prj.name}/src/opfor/tinygltf.cpp",
		"%{prj.name}/src/opfor/renderer/Texture.cpp",
		"%{prj.name}/src/opfor/renderer/TextureManager.cpp",
		"%{prj.name}/src/opfor/core/Application.cpp",
		"%{prj.name}/src/opfor/layers/LayerStack.cpp",
		"%{prj.name}/src/opfor/layers/ImGuiLayer.cpp",
		"%{prj.name}/src/opfor/utils/Settings.cpp",
		"%{prj.name}/src/opfor/ecs/ECSEngine.cpp",
		"%{prj.name}/src/opfor/ecs/Entity.cpp",
		"%{prj.name}/src/opfor/renderer/Mesh.cpp",
		"%{prj.name}/src/opfor/renderer/Batch.cpp",
		"%{prj.name}/src/opfor/core/ImageLoader.cpp",
		"%{prj.name}/src/opfor/renderer/GBuffer.cpp",
		"%{prj.name}/src/opfor/core/Window.cpp",
        "%{prj.name}/src/opfor/core/Input.cpp",
		"%{prj.name}/src/opfor/renderer/Context.cpp",
		"%{prj.name}/src/opfor/renderer/Renderer.cpp",
		"%{prj.name}/src/opfor/renderer/RendererAPI.cpp",
		"%{prj.name}/src/opfor/renderer/VertexArray.cpp",
		"%{prj.name}/src/opfor/renderer/Buffer.cpp",
		"%{prj.name}/src/opfor/renderer/RendererAPI.cpp",
		"%{prj.name}/src/opfor/renderer/RenderCommand.cpp",
		"%{prj.name}/src/opfor/renderer/Texture.cpp",
		"%{prj.name}/src/opfor/renderer/Renderbuffer.cpp",
		"%{prj.name}/src/opfor/renderer/Framebuffer.cpp",
		"%{prj.name}/src/opfor/renderer/Shader.cpp",
		"%{prj.name}/src/opfor/renderer/SceneRenderer.cpp",
		"%{prj.name}/src/opfor/renderer/SkyboxRenderer.cpp",
		"%{prj.name}/src/opfor/renderer/MeshRenderer.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLContext.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLVertexArray.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLBuffer.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLRendererAPI.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLTexture.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLRenderbuffer.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLFramebuffer.cpp",
		"%{prj.name}/src/opfor/platform/opengl/OpenGLShader.cpp",
		"%{prj.name}/src/opfor/platform/opengl/imgui_impl_opengl3.cpp",
		"%{prj.name}/src/opfor/platform/opengl/imgui_impl_glfw.cpp",
		"%{prj.name}/src/opfor/platform/linux/LinuxWindow.cpp",
		"%{prj.name}/src/opfor/platform/linux/LinuxInput.cpp",
		"%{prj.name}/src/opfor/platform/windows/WindowsInput.cpp",
		"%{prj.name}/src/opfor/platform/windows/WindowsWindow.cpp",
    }

    includedirs
    {
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.tinygltf}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stduuid}",
		"%{IncludeDir.NFD}",
		"%{IncludeDir.ImGuizmo}",
        "%{prj.name}/src",
        "%{prj.name}/src/opfor",
    }

	links
	{
        "GLFW",
        "GLAD",
        "fmt",
        "ImGui",
		"ImGuizmo",
		"nativefiledialog",
		"opengl32.lib"
	}

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_PLATFORM_OPENGL"
        }

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "OP4_DIST"
        optimize "On"

project "opfor-editor"
    location "opfor-editor"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.fmt}",
        "%{IncludeDir.GLAD}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.stduuid}",
        "opfor/src",
        "opfor/src/opfor",
        "opfor/thirdparty",
        "opfor/thirdparty/imgui",
        "opfor/thirdparty/ImGuizmo",
        "opfor/thirdparty/tinygltf",
        "opfor/thirdparty/nativefiledialog/src/include",
        "opfor/thirdparty/stduuid/include",
        "opfor/thirdparty/glew/include",
        "opfor/thirdparty/glm",
    }

    links
    {
        "opfor",
        "fmt",
        "ImGui"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_PLATFORM_OPENGL"
        }

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "OP4_DIST"
        optimize "On"
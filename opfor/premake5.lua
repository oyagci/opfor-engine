project "opfor"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.hpp",
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        ".",
        "src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.tinygltf}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stduuid}",
        "%{IncludeDir.NFD}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.RefurekuLib}",
    }

    links
    {
        "GLFW",
        "GLAD",
        "fmt",
        "ImGui",
        "ImGuizmo",
        "nativefiledialog",
        "opengl32.lib",
        "Kodgen",
        "RefurekuLibrary",
        "RefurekuGenerator",
    }

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Dist"
        defines "OP4_DIST"
        optimize "On"

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_BACKEND_OPENGL"
        }

        prebuildcommands
        {
            path.translate("%{wks.location}bin/" .. outputdir .. "/RefurekuGenerator/RefurekuGenerator.exe") .. " " .. path.translate("%{prj.location}config"),
        }

    filter "system:linux"
        defines
        {
            "OP4_PLATFORM_LINUX",
            "OP4_BACKEND_OPENGL"
        }

        prebuildcommands
        {
            ("LD_LIBRARY_PATH=%{wks.location}/bin/" .. outputdir .. "/Kodgen/  %{wks.location}/bin/" .. outputdir .. "/RefurekuGenerator/RefurekuGenerator %{prj.location}/config")
        }

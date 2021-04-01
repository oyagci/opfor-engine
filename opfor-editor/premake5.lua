project "GenerateHeadersEditor"
    kind "Utility"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    links {
        "RefurekuGenerator"
    }

    filter "system:windows"
        prebuildcommands
        {
            path.translate("%{wks.location}/bin/" .. outputdir .. "/RefurekuGenerator/RefurekuGenerator.exe %{prj.location}/config")
        }

    filter "system:linux"
        prebuildcommands
        {
            ("LD_LIBRARY_PATH=%{wks.location}/bin/" .. outputdir .. "/Kodgen/  %{wks.location}/bin/" .. outputdir .. "/RefurekuGenerator/RefurekuGenerator %{prj.location}/config")
        }

project "opfor-editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp"
    }

    includedirs
    {
        ".",
        "%{wks.location}/opfor",
        "%{wks.location}/opfor/include",
        "%{wks.location}/opfor/src",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.stduuid}",
        "%{IncludeDir.tinygltf}",
        "%{IncludeDir.RefurekuLib}",
    }

    links
    {
        "opfor",
        "GenerateHeadersEditor",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_BACKEND_OPENGL"
        }

    filter "system:linux"
        defines
        {
            "OP4_PLATFORM_LINUX",
            "OP4_BACKEND_OPENGL"
        }

        links
        {
            "dl",
            "m",
            "X11",
            "pthread",
            "GL",
            "fmt",
            "ImGui",
            "uuid",
            "GLFW",
            "GLAD",
            "ImGui",
            "ImGuizmo",
            "nativefiledialog",
            "Kodgen",
            "RefurekuLibrary",
        }

        linkoptions {"`pkg-config --libs gtk+-3.0`"}

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Dist"
        defines "OP4_DIST"
        optimize "On"

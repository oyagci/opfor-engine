project "GenerateHeaders"
    kind "Utility"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    links {
        "RefurekuGenerator"
    }

	filter "system:windows"
        prebuildcommands {
            path.translate("%{wks.location}bin/" .. outputdir .. "/RefurekuGenerator/RefurekuGenerator.exe") .. " " .. path.translate("%{prj.location}config"),
        }

    filter "system:linux"
        prebuildcommands {
            ("LD_LIBRARY_PATH=%{wks.location}/bin/" .. outputdir .. "/Kodgen/  %{wks.location}/bin/" .. outputdir .. "/RefurekuGenerator/RefurekuGenerator %{prj.location}/config")
        }

project "opfor-tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.hpp",
        "src/**.h",
        "src/**.cpp",
        "%{wks.location}/opfor/vendor/googletest/googletest/src/gtest_main.cc",
    }

    includedirs {
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
        "%{IncludeDir.googletest}",
    }

    links {
        "GLFW",
        "GLAD",
        "fmt",
        "ImGui",
        "ImGuizmo",
        "nativefiledialog",
        "Kodgen",
        "RefurekuLibrary",
        "GenerateHeaders",
        "googletest",
    }

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "system:windows"
        systemversion "latest"

        links {
			"opengl32.lib",
        }


    filter "system:linux"
		links {
			"pthread"
		}

workspace "opfor"
    architecture "x64"
    startproject "opfor-editor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
     "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"]     = "%{wks.location}/opfor/vendor/glfw/include"
IncludeDir["GLAD"]     = "%{wks.location}/opfor/vendor/glad/include"
IncludeDir["fmt"]      = "%{wks.location}/opfor/vendor/fmt/include"
IncludeDir["ImGui"]    = "%{wks.location}/opfor/vendor/imgui"
IncludeDir["tinygltf"] = "%{wks.location}/opfor/vendor/tinygltf"
IncludeDir["glm"]      = "%{wks.location}/opfor/vendor/glm"
IncludeDir["stduuid"]  = "%{wks.location}/opfor/vendor/stduuid/include"
IncludeDir["NFD"]      = "%{wks.location}/opfor/vendor/nativefiledialog/src/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/opfor/vendor/ImGuizmo"

group "dependencies"
    include "opfor/vendor/glfw"
    include "opfor/vendor/glad"
    include "opfor/vendor/fmt"
    include "opfor/vendor/imgui"
    include "opfor/vendor/ImGuizmo"
    include "opfor/vendor/nativefiledialog"
group ""

include "opfor"
include "opfor-editor"

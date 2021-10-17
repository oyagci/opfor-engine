-- Remove after alpha16 release
if not os.commandTokens._.copyfile then
  os.commandTokens._.copyfile = function(v)
    return "cp -f " .. path.normalize(v)
  end
  os.commandTokens._.copydir = function(v)
    return "cp -rf " .. path.normalize(v)
  end

  os.commandTokens.windows.copyfile = function(v)
    v = path.translate(path.normalize(v))
    return "copy /B /Y " .. v
  end
  os.commandTokens.windows.copydir = function(v)
    v = path.translate(path.normalize(v))
    return "xcopy /Q /E /Y /I " .. v
  end
end

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
IncludeDir["GLFW"]        = "%{wks.location}/opfor/vendor/glfw/include"
IncludeDir["GLAD"]        = "%{wks.location}/opfor/vendor/glad/include"
IncludeDir["fmt"]         = "%{wks.location}/opfor/vendor/fmt/include"
IncludeDir["ImGui"]       = "%{wks.location}/opfor/vendor/imgui"
IncludeDir["tinygltf"]    = "%{wks.location}/opfor/vendor/tinygltf"
IncludeDir["glm"]         = "%{wks.location}/opfor/vendor/glm"
IncludeDir["stduuid"]     = "%{wks.location}/opfor/vendor/stduuid/include"
IncludeDir["NFD"]         = "%{wks.location}/opfor/vendor/nativefiledialog/src/include"
IncludeDir["ImGuizmo"]    = "%{wks.location}/opfor/vendor/ImGuizmo"
IncludeDir["RefurekuLib"] = "%{wks.location}/opfor/vendor/Refureku/Refureku/Library/Include"
IncludeDir["googletest"]  = "%{wks.location}/opfor/vendor/googletest/googletest/include"

group "dependencies"
    include "opfor/vendor/glfw"
    include "opfor/vendor/glad"
    include "opfor/vendor/fmt"
    include "opfor/vendor/imgui"
    include "opfor/vendor/ImGuizmo"
    include "opfor/vendor/nativefiledialog"
    include "opfor/vendor/Refureku/Refureku/Generator"
    include "opfor/vendor/Refureku/Refureku/Library"
    include "opfor/vendor/googletest/googletest"
group ""

include "opfor"
include "opfor-editor"
include "opfor-tests"

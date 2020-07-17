project('ft_vox', 'cpp',
	version: '0.1',
	default_options: [
	  'warning_level=3',
	  'cpp_std=gnu++17',
	  'cpp_args=-Wno-class-memaccess',
	  'c_std=gnu17',
	  'c_args=-Wno-pedantic -Wno-unused-parameter -Wno-sign-compare -Wno-format-truncation',
	  'werror=false',
	  'optimization=2'])

add_project_arguments('-DOP4_PLATFORM_LINUX', language: 'cpp')

conf_data = configuration_data()
conf_data.set('version', 'v0.0.2')
configure_inc = '.'

configure_file(input: 'config.h.in',
			   output: 'config.h',
			   configuration: conf_data)

incdirs = []
incdirs += include_directories(configure_inc)
incdirs += include_directories('src')
incdirs += include_directories('src/engine')
incdirs += include_directories('thirdparty')
incdirs += include_directories('thirdparty/imgui')
incdirs += include_directories('thirdparty/ImGuizmo')
incdirs += include_directories('thirdparty/tinygltf')
incdirs += include_directories('thirdparty/lua-5.3.5/src')
incdirs += include_directories('thirdparty/nativefiledialog/src/include')
incdirs += include_directories('thirdparty/stduuid/include')
incdirs += include_directories('thirdparty/glew/include')
incdirs += include_directories('thirdparty/glm')

srcs = [
  'src/main.cpp',
  'src/stb_image.cpp',
  'src/engine/Texture.cpp',
  'src/engine/TextureManager.cpp',
  'src/engine/Cubemap.cpp',
  'src/engine/Engine.cpp',
  'src/engine/utils/Settings.cpp',
  'src/engine/ecs/ECSEngine.cpp',
  'src/engine/ecs/Entity.cpp',
  'src/engine/ui/TextRenderer.cpp',
  'src/engine/ui/Anchor.cpp',
  'src/engine/ui/Button.cpp',
  'src/engine/ui/UI.cpp',
  'src/engine/ui/SceneComponent.cpp',
  'src/engine/ui/UIScene.cpp',
  'src/engine/Framebuffer.cpp',
  'src/engine/Mesh.cpp',
  'src/engine/Batch.cpp',
  'src/engine/lualib.cpp',

  'src/engine/core/Window.cpp',
  'src/engine/core/Input.cpp',
  'src/engine/renderer/Renderer.cpp',
  'src/engine/renderer/Buffer.cpp',
  'src/engine/platform/opengl/OpenGLContext.cpp',
  'src/engine/platform/opengl/OpenGLBuffer.cpp',
  'src/engine/platform/linux/LinuxWindow.cpp',
  'src/engine/platform/linux/LinuxInput.cpp',
]

imgui = [
  'thirdparty/imgui/imgui.cpp',
  'thirdparty/imgui/imgui_demo.cpp',
  'thirdparty/imgui/imgui_impl_glfw.cpp',
  'thirdparty/imgui/imgui_widgets.cpp',
  'thirdparty/imgui/imgui_impl_opengl3.cpp',
  'thirdparty/imgui/imgui_draw.cpp',
  'thirdparty/imgui/misc/cpp/imgui_stdlib.cpp',
]

imgui_guizmo = [
  'thirdparty/ImGuizmo/ImGradient.cpp',
  'thirdparty/ImGuizmo/ImGuizmo.cpp',
  'thirdparty/ImGuizmo/ImSequencer.cpp',
  'thirdparty/ImGuizmo/ImCurveEdit.cpp',
]

tinygltf_src = [
  'src/engine/tinygltf.cpp'
]

nativefiledialog_src = [
  'thirdparty/nativefiledialog/src/nfd_common.c',
  'thirdparty/nativefiledialog/src/nfd_gtk.c'
]

glew_src = [
  'thirdparty/glew/src/glew.c'
]

srcs += imgui
srcs += imgui_guizmo
srcs += tinygltf_src
srcs += nativefiledialog_src
srcs += glew_src

cmake = import('cmake')
lazyGL = cmake.subproject('LazyGL')
json = cmake.subproject('json')
glfw = cmake.subproject('glfw')

# Lua
lua_src = [
  'thirdparty/lua-5.3.5/src/lfunc.c',
  'thirdparty/lua-5.3.5/src/lopcodes.c',
  'thirdparty/lua-5.3.5/src/lauxlib.c',
  'thirdparty/lua-5.3.5/src/ltm.c',
  'thirdparty/lua-5.3.5/src/lbitlib.c',
  'thirdparty/lua-5.3.5/src/ldo.c',
  'thirdparty/lua-5.3.5/src/lgc.c',
  'thirdparty/lua-5.3.5/src/liolib.c',
  'thirdparty/lua-5.3.5/src/lobject.c',
  'thirdparty/lua-5.3.5/src/lmathlib.c',
  'thirdparty/lua-5.3.5/src/lstate.c',
  'thirdparty/lua-5.3.5/src/ltable.c',
  'thirdparty/lua-5.3.5/src/ldump.c',
  'thirdparty/lua-5.3.5/src/lcorolib.c',
  'thirdparty/lua-5.3.5/src/lua.c',
  'thirdparty/lua-5.3.5/src/llex.c',
  'thirdparty/lua-5.3.5/src/lundump.c',
  'thirdparty/lua-5.3.5/src/lmem.c',
  'thirdparty/lua-5.3.5/src/lbaselib.c',
  'thirdparty/lua-5.3.5/src/ldblib.c',
  'thirdparty/lua-5.3.5/src/lctype.c',
  'thirdparty/lua-5.3.5/src/lutf8lib.c',
  'thirdparty/lua-5.3.5/src/lvm.c',
  'thirdparty/lua-5.3.5/src/lzio.c',
  'thirdparty/lua-5.3.5/src/lstrlib.c',
  'thirdparty/lua-5.3.5/src/lapi.c',
  'thirdparty/lua-5.3.5/src/lstring.c',
  'thirdparty/lua-5.3.5/src/loslib.c',
  'thirdparty/lua-5.3.5/src/lcode.c',
  'thirdparty/lua-5.3.5/src/ltablib.c',
  'thirdparty/lua-5.3.5/src/lparser.c',
  'thirdparty/lua-5.3.5/src/loadlib.c',
  'thirdparty/lua-5.3.5/src/linit.c',
  'thirdparty/lua-5.3.5/src/ldebug.c',
]

lua_deps = [ meson.get_compiler('c').find_library('m') ]
lua_inc = include_directories('thirdparty/lua-5.3.5/src')
lib_lua = library('lua', lua_src, include_directories: lua_inc, dependencies: lua_deps)

deps = [
  lazyGL.dependency('LazyGL'),
  json.dependency('nlohmann_json'),
  glfw.dependency('glfw'),
  dependency('threads', required : true),
  dependency('GL', required : true),
  dependency('fmt', required: true),
  dependency('uuid', required: true),
  meson.get_compiler('cpp').find_library('dl'),
  meson.get_compiler('cpp').find_library('m'),
]

incdirs += [ include_directories('.') ]

if build_machine.system() == 'linux'
  deps_linux = [
	dependency('x11', required : true),
	dependency('xi', required : true),
	dependency('xrandr', required : true),
	dependency('gtk+-3.0', required: true),
	meson.get_compiler('cpp').find_library('freetype'),
  ]
  deps += deps_linux
  incdirs += include_directories('/usr/include/freetype2/')
endif
if build_machine.system() == 'darwin'
  deps_macos = [
	meson.get_compiler('cpp')
	  .find_library('freetype', dirs: '~/.brew/lib')
  ]
  deps += deps_macos
  incdirs += include_directories('/Users/oyagci/.brew/include/freetype2') 
endif

#subdir('tests')

executable('ft_vox',
  srcs,
  include_directories : incdirs,
  dependencies: deps,
  link_with: lib_lua
)

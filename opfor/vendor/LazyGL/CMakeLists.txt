cmake_minimum_required(VERSION 3.5)

set(NAME LazyGL)
project(${NAME})

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw)

set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
set(BUILD_FRAMEWORK ON CACHE BOOL "" FORCE)
set(CMAKE_INSTALL_PREFIX "/Library/Frameworks")
add_subdirectory(dependencies/glew/build/cmake)

file(GLOB_RECURSE SOURCES sources/*)
file(GLOB_RECURSE HEADERS includes/*)

add_library(${NAME} ${SOURCES} ${HEADERS})

if(APPLE)
	find_library(COCOA_LIBRARY Cocoa REQUIRED)
	find_library(IOKIT_LIBRARY IOKit REQUIRED)
	find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)

	set(LIBRARIES
		${COCOA_LIBRARY}
		${IOKIT_LIBRARY}
		${COREVIDEO_LIBRARY}
		${OPENGL_gl_LIBRARY}
		glfw
		glew)
elseif(WINDOWS)
	set(LIBRARIES
		${OPENGL_gl_LIBRARY}
		glew
		glfw
	)
else()
	set(LIBRARIES
		${OPENGL_gl_LIBRARY}
		glfw
		glew)
endif()

set(INCLUDES
	sources/
	includes/
	dependencies/glew/include
	dependencies/glfw/include
	dependencies/glm/)

target_link_libraries(${NAME} ${LIBRARIES})
target_include_directories(${NAME} PUBLIC ${INCLUDES})

# --- TestGame compilation ---
# add_executable(TestGame tests/TestGame.cpp)
# target_link_libraries(TestGame ${NAME})
# target_include_directories(TestGame PUBLIC includes/)

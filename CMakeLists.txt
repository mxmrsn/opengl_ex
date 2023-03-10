cmake_minimum_required(VERSION 3.0.0)
project(opengl_ex VERSION 0.1.0)

set (EIGEN3_INCLUDE_DIR /usr/local/include/eigen3)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

include_directories(
    include
    ${EIGEN3_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
)

add_library(helperLib
	include/common/shader.cpp
	include/common/shader.hpp
	include/common/controls.cpp
	include/common/controls.hpp
	include/common/texture.cpp
	include/common/texture.hpp
	include/common/objloader.cpp
	include/common/objloader.hpp
	include/common/vboindexer.cpp
	include/common/vboindexer.hpp
	include/common/text2D.hpp
	include/common/text2D.cpp
	include/common/tangentspace.hpp
	include/common/tangentspace.cpp
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
	helperLib
)

add_executable(render_mesh src/render_mesh.cpp)
target_link_libraries(render_mesh ${ALL_LIBS})

add_executable(rotating_camera src/rotating_camera.cpp)
target_link_libraries(rotating_camera ${ALL_LIBS})
cmake_minimum_required(VERSION 3.28)
project(openglTests C)

set(CMAKE_C_STANDARD 99)

# Find GLFW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# Find OpenGL ES 2.0
find_library(GLESv2_LIBRARY NAMES libGLESv2 GLESv2)

# Include directories
include_directories(${GLFW_INCLUDE_DIRS} include)

# Link libraries to each executable
add_executable(glBlendFuncSelected src/glBlendFuncSelected.c)
target_link_libraries(glBlendFuncSelected ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(glBlendFunc src/glBlendFunc.c)
target_link_libraries(glBlendFunc ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(glBlendEquation src/glBlendEquation.c)
target_link_libraries(glBlendEquation ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(glBlendFuncSeparate src/glBlendFuncSeparate.c)
target_link_libraries(glBlendFuncSeparate ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(glBlendEquationSeparate src/glBlendEquationSeparate.c)
target_link_libraries(glBlendEquationSeparate ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(glGetError src/glGetError.c)
target_link_libraries(glGetError ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(fragment_variables src/fragment_variables.c)
target_link_libraries(fragment_variables ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(glsl_limits_test src/glsl_limits_test.c)
target_link_libraries(glsl_limits_test ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(qualifiers src/qualifiers.c)
target_link_libraries(qualifiers ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

add_executable(vertex_variables src/vertex_variables.c)
target_link_libraries(vertex_variables ${GLFW_LIBRARIES} ${GLESv2_LIBRARY})

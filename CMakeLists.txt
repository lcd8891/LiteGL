cmake_minimum_required(VERSION 3.10)
project (LiteGL)

find_package(glfw3 REQUIRED)
find_package(SFML CONFIG COMPONENTS graphics system REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_library(lite-buffers SHARED src/buffer/buffer.cpp)
add_library(lite-graphic SHARED src/graphics/shaderconstructor.cpp src/graphics/mesh.cpp src/graphics/shader.cpp src/graphics/texture.cpp)
add_library(lite-window SHARED src/window/window.cpp)
add_library(lite-system SHARED src/logger.cpp src/time.cpp src/gameldr/gameldr.cpp)
add_library(lite-glctrl SHARED src/glctrl/glctrl.cpp)

add_library(lite-api SHARED nothing.cpp)

add_executable(${PROJECT_NAME} src/LiteGL.cpp)

include_directories(${CMAKE_HOME_DIRECTORY}/include)

target_link_libraries(lite-glctrl PRIVATE OpenGL::GL)
target_link_libraries(lite-buffers PRIVATE lite-graphic OpenGL::GL GLEW::GLEW lite-system)
target_link_libraries(lite-window PRIVATE glfw OpenGL::GL GLEW::GLEW)
target_link_libraries(lite-graphic PRIVATE OpenGL::GL GLEW::GLEW lite-system)

target_link_libraries(lite-api PUBLIC lite-window lite-system lite-buffers lite-glctrl)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL GLEW::GLEW lite-api)

install(TARGETS lite-api lite-buffers lite-graphic lite-window lite-system lite-glctrl EXPORT LiteGLTargets LIBRARY DESTINATION out/lib ARCHIVE DESTINATION out/lib RUNTIME DESTINATION out/bin INCLUDES DESTINATION out/include)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT LiteGLTargets
    FILE LiteGLConfig.cmake
    NAMESPACE LiteGL::
    DESTINATION lib/cmake/LiteGL
)
include(CMakePackageConfigHelpers)
write_basic_package
_version_file(
    LiteGLConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LiteGLConfigVersion.cmake
    DESTINATION out/lib/cmake/LiteGL
)
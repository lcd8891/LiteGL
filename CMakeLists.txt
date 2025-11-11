cmake_minimum_required(VERSION 3.10)
project (LiteGL VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_PREFIX C:/msys64/mingw64)

find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_library(lite-buffers OBJECT src/buffer/buffer.cpp)
add_library(lite-graphic OBJECT src/graphics/shaderconstructor.cpp src/graphics/mesh.cpp src/graphics/shader.cpp src/graphics/texture.cpp src/graphics/vertexarray.cpp)
add_library(lite-window OBJECT src/window/window.cpp src/window/exception.cpp)
add_library(lite-system OBJECT src/system/logger.cpp src/system/time.cpp src/gameldr/gameldr.cpp src/system/types.cpp src/system/arguments.cpp)
add_library(lite-glctrl OBJECT src/glctrl/glctrl.cpp)
add_library(lite-defaults OBJECT src/system/default.cpp)
add_library(lite-screen OBJECT src/screen/screenmgr.cpp)

add_library(lite-api SHARED 
    $<TARGET_OBJECTS:lite-buffers>
    $<TARGET_OBJECTS:lite-graphic>
    $<TARGET_OBJECTS:lite-window>
    $<TARGET_OBJECTS:lite-system>
    $<TARGET_OBJECTS:lite-glctrl>
    $<TARGET_OBJECTS:lite-defaults>
    $<TARGET_OBJECTS:lite-screen>
)
target_include_directories(lite-api
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(lite-api PUBLIC OpenGL::GL GLEW::GLEW Freetype::Freetype glfw)

if(WIN32)
add_executable(${PROJECT_NAME} WIN32 src/LiteGL.cpp)
else()
add_executable(${PROJECT_NAME} src/LiteGL.cpp)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE lite-api)

include_directories(${CMAKE_HOME_DIRECTORY}/include)

install(FILES 
    ${CMAKE_BINARY_DIR}/lite-api_export.h
    DESTINATION include
)
install(TARGETS lite-api 
    EXPORT LiteGLTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT LiteGLTargets
    FILE LiteGLConfig.cmake
    NAMESPACE LiteGL::
    DESTINATION lib/cmake/LiteGL
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/LiteGLConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    ${CMAKE_BINARY_DIR}/LiteGLConfigVersion.cmake
    DESTINATION lib/cmake/LiteGL
)
add_custom_command(TARGET lite-api POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/include/lite-api_export.h
        ${CMAKE_HOME_DIRECTORY}/out/include/
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/LiteGLConfigVersion.cmake
        ${CMAKE_HOME_DIRECTORY}/out/lib/cmake/LiteGL/
    COMMENT "Copying export header and config files to output directory"
)
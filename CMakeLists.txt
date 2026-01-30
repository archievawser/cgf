cmake_minimum_required(VERSION 3.6)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
project(cgf CXX)

set(SOURCE
 	"src/graphics/Shader.cpp"
	"src/graphics/Renderer.cpp"
	"src/graphics/Material.cpp"
	"src/core/Game.cpp"
	"src/core/AssetLibrary.cpp"
	"src/core/Window.cpp"
)

add_library(cgf ${SOURCE})

target_compile_definitions(cgf PRIVATE UNICODE)
target_compile_definitions(cgf PRIVATE ENGINE_DLL=1)

FetchContent_Declare(
	dcore
	GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
)

FetchContent_MakeAvailable(dcore)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
)

FetchContent_MakeAvailable(assimp)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
)

FetchContent_MakeAvailable(glfw)

target_include_directories(cgf 
	PUBLIC "include/cgf" 
	PUBLIC ${dcore_SOURCE_DIR} 
	PUBLIC ${assimp_SOURCE_DIR}/include 
	PUBLIC ${glfw_SOURCE_DIR}/include)

target_link_libraries(cgf PUBLIC Diligent-BuildSettings glfw)

if(D3D11_SUPPORTED)
	target_link_libraries(cgf PUBLIC Diligent-GraphicsEngineD3D11-shared)
endif()

if(D3D12_SUPPORTED)
	target_link_libraries(cgf PUBLIC Diligent-GraphicsEngineD3D12-shared)
endif()

if(GL_SUPPORTED)
	target_link_libraries(cgf PUBLIC Diligent-GraphicsEngineOpenGL-shared)
endif()

if(VULKAN_SUPPORTED)
	target_link_libraries(cgf PUBLIC Diligent-GraphicsEngineVk-shared)
endif()

message(${CMAKE_CURRENT_BINARY_DIR})

copy_required_dlls(cgf)

add_subdirectory(samples/dev/)
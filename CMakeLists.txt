cmake_minimum_required(VERSION 3.6)
include(FetchContent)

project(HelloWorld CXX)

set(SOURCE
	"src/Main.cpp"
 	"src/graphics/Shader.cpp"
	"src/core/Game.cpp")

add_executable(HelloWorld WIN32 ${SOURCE})

target_compile_definitions(HelloWorld PRIVATE UNICODE)

target_compile_definitions(HelloWorld PRIVATE ENGINE_DLL=1)

FetchContent_Declare(
	DiliCore
	GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
)

FetchContent_MakeAvailable(DiliCore)

target_include_directories(HelloWorld PUBLIC ${dilicore_SOURCE_DIR} PUBLIC "include")
target_link_libraries(HelloWorld PRIVATE Diligent-BuildSettings)

if(D3D11_SUPPORTED)
	target_link_libraries(HelloWorld PRIVATE Diligent-GraphicsEngineD3D11-shared)
endif()

if(D3D12_SUPPORTED)
	target_link_libraries(HelloWorld PRIVATE Diligent-GraphicsEngineD3D12-shared)
endif()

if(GL_SUPPORTED)
	target_link_libraries(HelloWorld PRIVATE Diligent-GraphicsEngineOpenGL-shared)
endif()

if(VULKAN_SUPPORTED)
	target_link_libraries(HelloWorld PRIVATE Diligent-GraphicsEngineVk-shared)
endif()

copy_required_dlls(HelloWorld)
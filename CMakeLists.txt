cmake_minimum_required(VERSION 3.6)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
project(general-engine CXX)

set(SOURCE
	"src/Main.cpp"
 	"src/graphics/Shader.cpp"
	"src/graphics/Renderer.cpp"
	"src/core/Game.cpp"
)

add_executable(general-engine WIN32 ${SOURCE})

target_compile_definitions(general-engine PRIVATE UNICODE)
target_compile_definitions(general-engine PRIVATE ENGINE_DLL=1)

FetchContent_Declare(
	dcore
	GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
)

FetchContent_MakeAvailable(dcore)

target_include_directories(general-engine PUBLIC ${dcore_SOURCE_DIR} PUBLIC "include")

target_link_libraries(general-engine PRIVATE Diligent-BuildSettings)

if(D3D11_SUPPORTED)
	target_link_libraries(general-engine PRIVATE Diligent-GraphicsEngineD3D11-shared)
endif()

if(D3D12_SUPPORTED)
	target_link_libraries(general-engine PRIVATE Diligent-GraphicsEngineD3D12-shared)
endif()

if(GL_SUPPORTED)
	target_link_libraries(general-engine PRIVATE Diligent-GraphicsEngineOpenGL-shared)
endif()

if(VULKAN_SUPPORTED)
	target_link_libraries(general-engine PRIVATE Diligent-GraphicsEngineVk-shared)
endif()

copy_required_dlls(general-engine)
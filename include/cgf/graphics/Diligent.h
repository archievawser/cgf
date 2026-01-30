#pragma once

#include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include "Common/interface/RefCntAutoPtr.hpp"

using namespace Diligent;

#ifndef D3D11_SUPPORTED
#define D3D11_SUPPORTED 0
#endif

#ifndef D3D12_SUPPORTED
#define D3D12_SUPPORTED 0
#endif

#ifndef GL_SUPPORTED
#define GL_SUPPORTED 0
#endif

#ifndef VULKAN_SUPPORTED
#define VULKAN_SUPPORTED 0
#endif

#ifndef METAL_SUPPORTED
#define METAL_SUPPORTED 0
#endif
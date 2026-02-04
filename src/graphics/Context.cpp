#include "graphics/Context.h"


GraphicsContext::GraphicsContext(Window* window)
{
	auto Window = window->GetNativeWindowHandle();

	SwapChainDesc SCDesc;

	switch (m_DeviceType)
	{
#if D3D11_SUPPORTED
	case RENDER_DEVICE_TYPE_D3D11:
	{
		EngineD3D11CreateInfo EngineCI;

		auto *GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
		auto *pFactoryD3D11 = GetEngineFactoryD3D11();
		pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_RenderDevice, &m_DeviceContext);
		pFactoryD3D11->CreateSwapChainD3D11(m_RenderDevice, m_DeviceContext, SCDesc, FullScreenModeDesc{}, Window, &m_SwapChain);
	}
	break;
#endif

#if D3D12_SUPPORTED
	case RENDER_DEVICE_TYPE_D3D12:
	{
		auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();

		EngineD3D12CreateInfo EngineCI;
		auto *pFactoryD3D12 = GetEngineFactoryD3D12();
		pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_RenderDevice, &m_DeviceContext);
		pFactoryD3D12->CreateSwapChainD3D12(m_RenderDevice, m_DeviceContext, SCDesc, FullScreenModeDesc{}, Window, &m_SwapChain);
	}
	break;
#endif

#if GL_SUPPORTED
	case RENDER_DEVICE_TYPE_GL:
	{
		auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
		auto *pFactoryOpenGL = GetEngineFactoryOpenGL();

		EngineGLCreateInfo EngineCI;
		EngineCI.Window = Window;
		pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_RenderDevice, &m_DeviceContext, SCDesc, &m_SwapChain);
	}
	break;
#endif

#if VULKAN_SUPPORTED
	case RENDER_DEVICE_TYPE_VULKAN:
	{
		auto GetEngineFactoryVk = LoadGraphicsEngineVk();
		EngineVkCreateInfo EngineCI;

		auto *pFactoryVk = GetEngineFactoryVk();
		pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_RenderDevice, &m_DeviceContext);

		if (!m_SwapChain)
		{
			pFactoryVk->CreateSwapChainVk(m_RenderDevice, m_DeviceContext, SCDesc, Window, &m_SwapChain);
		}
	}
	break;
#endif

	default:
		CGF_ERROR("Failed to initialize Diligent due to an unknown/unsupported device type");
		break;
	}

	CGF_ASSERT(m_RenderDevice && m_SwapChain, "Failed to initialize Diligent");
}
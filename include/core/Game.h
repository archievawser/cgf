#pragma once

#include <memory>
#include <iomanip>
#include <iostream>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <crtdbg.h>

#ifndef PLATFORM_WIN32
#define PLATFORM_WIN32 1
#endif

#ifndef ENGINE_DLL
#define ENGINE_DLL 1
#endif

#ifndef D3D11_SUPPORTED
#define D3D11_SUPPORTED 1
#endif

#ifndef D3D12_SUPPORTED
#define D3D12_SUPPORTED 1
#endif

#ifndef GL_SUPPORTED
#define GL_SUPPORTED 1
#endif

#ifndef VULKAN_SUPPORTED
#define VULKAN_SUPPORTED 1
#endif

#include "graphics/Diligent.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "core/Common.h"


class Game
{
public:
	Game(HWND window)
	{
		Instance = this;

		Singleton<Renderer>::Initialize(window);
		Singleton<Renderer>::Get()->Setup();
	}

	virtual ~Game() = default;

	void Render()
	{
		Singleton<Renderer>::Get()->Render();
	}

	void WindowResize(Uint32 Width, Uint32 Height)
	{
		if (SwapChain)
			SwapChain->Resize(Width, Height);
	}

	RENDER_DEVICE_TYPE GetDeviceType() const { return m_DeviceType; }

	RefCntAutoPtr<IBuffer> buffer;
	RefCntAutoPtr<IBuffer> vbuffer;
	std::shared_ptr<Shader> ps;
	std::shared_ptr<Shader> vs;
	static Game *Instance;
	RefCntAutoPtr<IShaderResourceBinding> binding;
	RefCntAutoPtr<IRenderDevice> RenderDevice;
	RefCntAutoPtr<IDeviceContext> DeviceContext;
	RefCntAutoPtr<ISwapChain> SwapChain;
	RefCntAutoPtr<IPipelineState> PipelineState;
	RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
};
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

#include "graphics/DiligentCommon.h"
#include "graphics/Shader.h"


static const char* VSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

void main(in  uint    VertId : SV_VertexID,
          out PSInput PSIn) 
{
    float4 Pos[3];
    Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
    Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
    Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

    float3 Col[3];
    Col[0] = float3(1.0, 0.0, 0.0); // red
    Col[1] = float3(0.0, 1.0, 0.0); // green
    Col[2] = float3(0.0, 0.0, 1.0); // blue

    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Col[VertId];
}
)";

// Pixel shader simply outputs interpolated vertex color
static const char* PSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

struct PSOutput
{ 
    float4 Color : SV_TARGET; 
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)";


class Game
{
public:
	Game()
	{
		Instance = this;
	}

	virtual ~Game() = default;
	virtual void Tick() = 0;
	virtual void Draw() = 0;

	bool InitializeDiligent(HWND hWnd)
	{
		Diligent::SwapChainDesc SCDesc;
		switch (m_DeviceType)
		{
#if D3D11_SUPPORTED
		case Diligent::RENDER_DEVICE_TYPE_D3D11:
		{
			Diligent::EngineD3D11CreateInfo EngineCI;
#if ENGINE_DLL
			// Load the dll and import GetEngineFactoryD3D11() function
			auto* GetEngineFactoryD3D11 = Diligent::LoadGraphicsEngineD3D11();
#endif
			auto* pFactoryD3D11 = GetEngineFactoryD3D11();
			pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &RenderDevice, &DeviceContext);
			Diligent::Win32NativeWindow Window{ hWnd };
			pFactoryD3D11->CreateSwapChainD3D11(RenderDevice, DeviceContext, SCDesc, Diligent::FullScreenModeDesc{}, Window, &SwapChain);
		}
		break;
#endif

#if D3D12_SUPPORTED
		case Diligent::RENDER_DEVICE_TYPE_D3D12:
		{
#if ENGINE_DLL
			// Load the dll and import GetEngineFactoryD3D12() function
			auto GetEngineFactoryD3D12 = Diligent::LoadGraphicsEngineD3D12();
#endif
			Diligent::EngineD3D12CreateInfo EngineCI;

			auto* pFactoryD3D12 = GetEngineFactoryD3D12();
			pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &RenderDevice, &DeviceContext);
			Diligent::Win32NativeWindow Window{ hWnd };
			pFactoryD3D12->CreateSwapChainD3D12(RenderDevice, DeviceContext, SCDesc, Diligent::FullScreenModeDesc{}, Window, &SwapChain);
		}
		break;
#endif

#if GL_SUPPORTED
		case Diligent::RENDER_DEVICE_TYPE_GL:
		{
			auto GetEngineFactoryOpenGL = Diligent::LoadGraphicsEngineOpenGL();
			auto* pFactoryOpenGL = GetEngineFactoryOpenGL();

			Diligent::EngineGLCreateInfo EngineCI;
			EngineCI.Window.hWnd = hWnd;

			pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &RenderDevice, &DeviceContext, SCDesc, &SwapChain);
		}
		break;
#endif

#if VULKAN_SUPPORTED
		case Diligent::RENDER_DEVICE_TYPE_VULKAN:
		{
			auto GetEngineFactoryVk = Diligent::LoadGraphicsEngineVk();
			Diligent::EngineVkCreateInfo EngineCI;

			auto* pFactoryVk = GetEngineFactoryVk();
			pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &RenderDevice, &DeviceContext);

			if (!SwapChain && hWnd != nullptr)
			{
				Diligent::Win32NativeWindow Window{ hWnd };
				pFactoryVk->CreateSwapChainVk(RenderDevice, DeviceContext, SCDesc, Window, &SwapChain);
			}
		}
		break;
#endif

		default:
			std::cerr << "Unknown/unsupported device type";
			return false;
			break;
		}

		return true;
	}

	void CreateResources()
	{
		// Pipeline state object encompasses configuration of all GPU stages

		Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;

		// Pipeline state name is used by the engine to report issues.
		// It is always a good idea to give objects descriptive names.
		PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

		// This is a graphics pipeline
		PSOCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

		// clang-format off
		// This tutorial will render to a single render target
		PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
		// Set render target format which is the format of the swap chain's color buffer
		PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = SwapChain->GetDesc().ColorBufferFormat;
		// Use the depth buffer format from the swap chain
		PSOCreateInfo.GraphicsPipeline.DSVFormat = SwapChain->GetDesc().DepthBufferFormat;
		// Primitive topology defines what kind of primitives will be rendered by this pipeline state
		PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		// No back face culling for this tutorial
		PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
		// Disable depth testing
		PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::False;
		// clang-format on

		Diligent::ShaderCreateInfo ShaderCI;
		// Tell the system that the shader source code is in HLSL.
		// For OpenGL, the engine will convert this into GLSL under the hood
		ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
		// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
		ShaderCI.Desc.UseCombinedTextureSamplers = true;
		// Create a vertex shader
		Diligent::RefCntAutoPtr<Diligent::IShader> pVS;
		{
			ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
			ShaderCI.EntryPoint = "main";
			ShaderCI.Desc.Name = "Triangle vertex shader";
			ShaderCI.Source = VSSource;
			RenderDevice->CreateShader(ShaderCI, &pVS);
		}

		// Create a pixel shader
		Diligent::RefCntAutoPtr<Diligent::IShader> pPS;
		{
			ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
			ShaderCI.EntryPoint = "main";
			ShaderCI.Desc.Name = "Triangle pixel shader";
			ShaderCI.Source = PSSource;
			RenderDevice->CreateShader(ShaderCI, &pPS);
		}

		// Finally, create the pipeline state
		PSOCreateInfo.pVS = pVS;
		PSOCreateInfo.pPS = pPS;
		RenderDevice->CreateGraphicsPipelineState(PSOCreateInfo, &PipelineState);
	}

	void Render()
	{
		// Set render targets before issuing any draw command.
		// Note that Present() unbinds the back buffer if it is set as render target.
		auto* pRTV = SwapChain->GetCurrentBackBufferRTV();
		auto* pDSV = SwapChain->GetDepthBufferDSV();
		DeviceContext->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		// Clear the back buffer
		const float ClearColor[] = { 0.f, 0.f, 0.f, 1.0f };
		// Let the engine perform required state transitions
		DeviceContext->ClearRenderTarget(pRTV, ClearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		DeviceContext->ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		// Set the pipeline state in the immediate context
		DeviceContext->SetPipelineState(PipelineState);

		// Typically we should now call CommitShaderResources(), however shaders in this example don't
		// use any resources.

		Diligent::DrawAttribs drawAttrs;
		drawAttrs.NumVertices = 3; // Render 3 vertices
		DeviceContext->Draw(drawAttrs);

		SwapChain->Present();
	}

	void WindowResize(Diligent::Uint32 Width, Diligent::Uint32 Height)
	{
		if (SwapChain)
			SwapChain->Resize(Width, Height);
	}

	Diligent::RENDER_DEVICE_TYPE GetDeviceType() const { return m_DeviceType; }

	static Game* Instance;
	Diligent::RefCntAutoPtr<Diligent::IRenderDevice> RenderDevice;
	Diligent::RefCntAutoPtr<Diligent::IDeviceContext> DeviceContext;
	Diligent::RefCntAutoPtr<Diligent::ISwapChain> SwapChain;
	Diligent::RefCntAutoPtr<Diligent::IPipelineState> PipelineState;
	Diligent::RENDER_DEVICE_TYPE m_DeviceType = Diligent::RENDER_DEVICE_TYPE_D3D11;
};
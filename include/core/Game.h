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

static const char *VSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
};

void Main(in float2 position : POSITION,
          out PSInput PSIn) 
{
    PSIn.Pos = float4(position, 1.0, 1.0);
}
)";

static const char* PSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
};

struct PSOutput
{ 
    float4 Color : SV_TARGET; 
};

cbuffer Abcdef
{
	float4 value;
};

void Main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = value;
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
		Diligent::ShaderCreateInfo ShaderCI;
		ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
		ShaderCI.Desc.UseCombinedTextureSamplers = true;

		vs = std::shared_ptr<Shader>(new Shader("triangle_vert_shader", VSSource, Diligent::SHADER_TYPE_VERTEX));
		ps = std::shared_ptr<Shader>(new Shader("triangle_frag_shader", PSSource, Diligent::SHADER_TYPE_PIXEL));

		Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;
		PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";
		PSOCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
		PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;
		PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
		PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = SwapChain->GetDesc().ColorBufferFormat;
		PSOCreateInfo.GraphicsPipeline.DSVFormat = SwapChain->GetDesc().DepthBufferFormat;
		PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
		PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::False;
		PSOCreateInfo.pVS = vs->GetHandle();
		PSOCreateInfo.pPS = ps->GetHandle();

		Diligent::LayoutElement LayoutElems[] =
		{
			{ "POSITION", 0, 0, 2, Diligent::VT_FLOAT32 }
		};

		PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
		PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = 1;

		RenderDevice->CreateGraphicsPipelineState(PSOCreateInfo, &PipelineState);

		PipelineState->CreateShaderResourceBinding(&binding);

		float e[4] { 
			0.f, 0.f, 1.f, 1.f 
		};

		Diligent::BufferData data (e, sizeof(e));
		Diligent::BufferDesc bufferDesc;
		bufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_NONE;
		bufferDesc.Name = "Abcdef Buffer";
		bufferDesc.Usage = Diligent::USAGE_DEFAULT;
		bufferDesc.Size = sizeof(e);
		bufferDesc.BindFlags = Diligent::BIND_FLAGS::BIND_UNIFORM_BUFFER;
		RenderDevice->CreateBuffer(bufferDesc, &data, &buffer);

		float vertexData[] {
			0.5f, 0.5f,
			-0.5f, 0.5f,
			-0.5f, -0.5f,
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f, 0.5f
		};

		Diligent::BufferData vdata (vertexData, sizeof(vertexData));
		Diligent::BufferDesc vbufferDesc;
		vbufferDesc.Name = "test";
		vbufferDesc.Size = sizeof(vertexData);
		vbufferDesc.Usage = Diligent::USAGE_IMMUTABLE;
		vbufferDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
		RenderDevice->CreateBuffer(vbufferDesc, &vdata, &vbuffer);

		binding->GetVariableByName(Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, "Abcdef")->Set(buffer);		
		
		Diligent::IBuffer* yo[] { vbuffer };
		DeviceContext->SetVertexBuffers(0, 1, yo, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
	
	}

	void Render()
	{
		const float ClearColor[] = { 0.f, 0.f, 0.f, 1.0f };
		
		auto* pRTV = SwapChain->GetCurrentBackBufferRTV();
		auto* pDSV = SwapChain->GetDepthBufferDSV();
		DeviceContext->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		
		DeviceContext->ClearRenderTarget(pRTV, ClearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		DeviceContext->ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		DeviceContext->SetPipelineState(PipelineState);

		DeviceContext->CommitShaderResources(binding, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


		Diligent::DrawAttribs drawAttrs;
		drawAttrs.NumVertices = 6;
		DeviceContext->Draw(drawAttrs);

		SwapChain->Present();
	}

	void WindowResize(Diligent::Uint32 Width, Diligent::Uint32 Height)
	{
		if (SwapChain)
			SwapChain->Resize(Width, Height);
	}

	Diligent::RENDER_DEVICE_TYPE GetDeviceType() const { return m_DeviceType; }

	Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer;
	Diligent::RefCntAutoPtr<Diligent::IBuffer> vbuffer;
	std::shared_ptr<Shader> ps;
	std::shared_ptr<Shader> vs;
	static Game *Instance;
	Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> binding;
	Diligent::RefCntAutoPtr<Diligent::IRenderDevice> RenderDevice;
	Diligent::RefCntAutoPtr<Diligent::IDeviceContext> DeviceContext;
	Diligent::RefCntAutoPtr<Diligent::ISwapChain> SwapChain;
	Diligent::RefCntAutoPtr<Diligent::IPipelineState> PipelineState;
	Diligent::RENDER_DEVICE_TYPE m_DeviceType = Diligent::RENDER_DEVICE_TYPE_D3D11;
};
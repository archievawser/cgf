#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "core/Common.h"
#include "graphics/Diligent.h"
#include "graphics/Shader.h"
#include "graphics/Material.h"

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

static const char *PSSource = R"(
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

/**
struct GBufferPass : RenderPass
{
	void OnExecution() override
	{
		for(v in Renderables)
		{

		}
	}

	RenderTarget2D* Color;
	RenderTarget2D* Position;
	RenderTarget2D* Normals;
	
}

class SceneRenderer : Renderer
{
	RenderGraph* BuildRenderGraph() override
	{
		RenderGraphFactory builder;

		builder.AddPass([&]() {
			CommandList.EnqueueDraw(PSO, SRB)
		})
	}
}
*/


/**
 * @brief Render passes are the individual steps executed to render each frame
 */
struct RenderPass
{
	RenderPass() = default;
	virtual void Run(class RenderGraph* graph) = 0;
};


/**
 * @brief The render graph of a scene organizes & optimizes the execution of render passes involved in rendering each frame
 */
class RenderGraph
{
public:
	RefCntAutoPtr<ITexture> MakeTexture2D(
		std::string name, 
		unsigned int width, 
		unsigned int height, 
		TEXTURE_FORMAT format,
		TextureData* data = nullptr);

	FORCEINLINE void Execute()
	{
		for (std::shared_ptr<RenderPass> pass : m_Passes)
		{
			pass->Run(this);
		}
	}

	FORCEINLINE void AddPass(std::shared_ptr<RenderPass> pass)
	{
		m_Passes.push_back(pass);
	}

private:
	std::vector<std::shared_ptr<RenderPass>> m_Passes;
};


class Renderer
{
public:
	Renderer(HWND wnd)
	{
		InitializeDiligent(wnd);
	}	

	void Setup()
	{
		auto vs = std::shared_ptr<Shader>(new Shader("triangle_vert_shader", VSSource, SHADER_TYPE_VERTEX));
		auto ps = std::shared_ptr<Shader>(new Shader("triangle_frag_shader", PSSource, SHADER_TYPE_PIXEL));

		material = std::make_shared<Material>(vs, ps);

		float e[4]{
			1.f,
			1.f,
			1.f,
			1.f};

		BufferData data(e, sizeof(e));
		BufferDesc bufferDesc;
		bufferDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		bufferDesc.Name = "Abcdef Buffer";
		bufferDesc.Usage = USAGE_DEFAULT;
		bufferDesc.Size = sizeof(e);
		bufferDesc.BindFlags = BIND_UNIFORM_BUFFER;
		m_RenderDevice->CreateBuffer(bufferDesc, &data, &buffer);
		material->GetFragmentVariable("Abcdef")->Set(buffer);

		float vertexData[]
		{
			0.5f, 0.5f,
			-0.5f, 0.5f,
			-0.5f, -0.5f,
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f, 0.5f
		};

		BufferData vdata(vertexData, sizeof(vertexData));
		BufferDesc vbufferDesc;
		vbufferDesc.Name = "test";
		vbufferDesc.Size = sizeof(vertexData);
		vbufferDesc.Usage = USAGE_IMMUTABLE;
		vbufferDesc.BindFlags = BIND_VERTEX_BUFFER;
		m_RenderDevice->CreateBuffer(vbufferDesc, &vdata, &vbuffer);

		IBuffer *yo[]{vbuffer};
		m_DeviceContext->SetVertexBuffers(0, 1, yo, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
	}
	
	FORCEINLINE RefCntAutoPtr<IRenderDevice> GetRenderDevice()
	{
		return m_RenderDevice;
	}

	FORCEINLINE RefCntAutoPtr<IDeviceContext> GetDeviceContext()
	{
		return m_DeviceContext;
	}

	FORCEINLINE RefCntAutoPtr<ISwapChain> GetSwapChain()
	{
		return m_SwapChain;
	}

	FORCEINLINE RefCntAutoPtr<IPipelineState> GetPipelineState()
	{
		return m_PipelineState;
	}

	void Render();

private: 
	bool InitializeDiligent(HWND hWnd)
	{
		SwapChainDesc SCDesc;
		switch (m_DeviceType)
		{
#if D3D11_SUPPORTED
		case RENDER_DEVICE_TYPE_D3D11:
		{
			EngineD3D11CreateInfo EngineCI;
#if ENGINE_DLL
			// Load the dll and import GetEngineFactoryD3D11() function
			auto *GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
#endif
			auto *pFactoryD3D11 = GetEngineFactoryD3D11();
			pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_RenderDevice, &m_DeviceContext);
			Win32NativeWindow Window{hWnd};
			pFactoryD3D11->CreateSwapChainD3D11(m_RenderDevice, m_DeviceContext, SCDesc, FullScreenModeDesc{}, Window, &m_SwapChain);
		}
		break;
#endif

#if D3D12_SUPPORTED
		case RENDER_DEVICE_TYPE_D3D12:
		{
#if ENGINE_DLL
			// Load the dll and import GetEngineFactoryD3D12() function
			auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#endif
			EngineD3D12CreateInfo EngineCI;

			auto *pFactoryD3D12 = GetEngineFactoryD3D12();
			pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_RenderDevice, &m_DeviceContext);
			Win32NativeWindow Window{hWnd};
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
			EngineCI.Window.hWnd = hWnd;

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

			if (!m_SwapChain && hWnd != nullptr)
			{
				Win32NativeWindow Window{hWnd};
				pFactoryVk->CreateSwapChainVk(m_RenderDevice, m_DeviceContext, SCDesc, Window, &m_SwapChain);
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

	RenderGraph m_CurrentGraph;
	RefCntAutoPtr<IRenderDevice> m_RenderDevice;
	RefCntAutoPtr<IDeviceContext> m_DeviceContext;
	RefCntAutoPtr<ISwapChain> m_SwapChain;
	RefCntAutoPtr<IPipelineState> m_PipelineState;
	RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
	std::shared_ptr<Material> material;
	RefCntAutoPtr<IBuffer> buffer;
	RefCntAutoPtr<IBuffer> vbuffer;
};

/*
void Test()
{
	RenderGraph graph;

	auto texture = graph.MakeTexture2D("Color", 1920, 1080, TEX_FORMAT_RGBA8_SINT);

	struct GBufferPass : public RenderPass
	{
		void Run(RenderGraph *graph) override
		{
			
		}
		
		RefCntAutoPtr<ITexture> target;
	};

	std::shared_ptr<GBufferPass> gBufferPass (new GBufferPass);
	gBufferPass->target = texture;

	graph.AddPass(gBufferPass);
}*/
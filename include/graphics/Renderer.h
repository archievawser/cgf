#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "core/Common.h"
#include "core/AssetLibrary.h"

#include "graphics/Diligent.h"
#include "graphics/Shader.h"
#include "graphics/Material.h"


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


/**
 * @brief Contains the information depended on by a majority of simple draw calls
 */
struct EntityDrawCmd
{
	EntityDrawCmd(
		RefCntAutoPtr<IBuffer> indexBuffer,
		RefCntAutoPtr<IBuffer> vertexBuffer,
		std::shared_ptr<MaterialInstance> material,
		int indexCount);

	int IndexCount;
	bool Valid = true;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	std::shared_ptr<MaterialInstance> DrawMaterial;
};


class EntityDrawCmdList;


/**
 * @brief A handle which invalidates the associated PersistentEntityDrawCmd upon destruction, effectively
 * tying their lifetimes.
 */
struct EntityDrawCmdHandle
{
	EntityDrawCmdHandle(int cmdIndex, EntityDrawCmdList* chain);
	~EntityDrawCmdHandle();

	int CmdIndex;
	EntityDrawCmdList* Chain;
};


/**
 * @brief An unordered list of draw commands, performed upon execution see DrawCommandChain::Execute()
 */
class EntityDrawCmdList
{
	friend class EntityDrawCmdHandle;

public:
	EntityDrawCmdList() = default;

	std::shared_ptr<EntityDrawCmdHandle> CreateCommand(
		RefCntAutoPtr<IBuffer> indexBuffer,
		RefCntAutoPtr<IBuffer> vertexBuffer,
		std::shared_ptr<MaterialInstance> material,
		int indexCount);

	/**
	 * @brief Executes all valid commands currently within the chain
	 */
	void Execute();

private:
	std::vector<EntityDrawCmd> m_CmdList;
};


/**
 * @brief Manages a majority of graphical operations performed by the engine, especially the
 * frame-by-frame visualization of scenes.
 */
class Renderer
{
public:
	Renderer(HWND wnd)
	{
		InitializeDiligent(wnd);
	}	

	void Render();

	void Execute(EntityDrawCmd& cmd);

	/**
	 * @brief Binds a graphics pipeline if the requested pipeline isn't already bound
	 */
	FORCEINLINE void UsePipeline(RefCntAutoPtr<IPipelineState> pipeline)
	{
		if (m_PipelineState != pipeline)
		{
			m_PipelineState = pipeline;
			m_DeviceContext->SetPipelineState(pipeline);
		}
	}

	void Setup()
	{
		Singleton<AssetLibrary>::Initialize();

		std::shared_ptr<Shader> fs ( Singleton<AssetLibrary>::Get()->Get<Shader*>("fPrototype.hlsl") );
		std::shared_ptr<Shader> vs ( Singleton<AssetLibrary>::Get()->Get<Shader*>("vPrototype.hlsl") );

		auto basematerial = std::make_shared<Material>(vs, fs);
		material1 = std::make_shared<MaterialInstance>(basematerial);
		material2 = std::make_shared<MaterialInstance>(basematerial);

		glm::mat4 proj = glm::perspectiveFov(70.f, 1920.f, 1080.f, 0.1f, 1000.f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -10));
		glm::mat4 model = glm::mat4x4(1.0f);

		glm::mat4x4 mvp1 = proj * view * glm::translate(model, glm::vec3(1, 0, 0));
		glm::mat4x4 mvp2 = proj * view * glm::translate(model, glm::vec3(-1, 0, 0));

		DeviceVarBinding<glm::mat4> binding1 = material1->CreateVertexVariableBinding<glm::mat4>("WorldData");
		binding1.Set(mvp1);

		DeviceVarBinding<glm::mat4> binding2 = material2->CreateVertexVariableBinding<glm::mat4>("WorldData");
		binding2.Set(mvp2);

		float vertexData[]
		{
			 0.5f,  0.5f,
			-0.5f,  0.5f,
			-0.5f, -0.5f,
			 0.5f, -0.5f
		};

		unsigned int indexData[]
		{
			0, 1, 2,
			2, 3, 0
		};

		RefCntAutoPtr<IBuffer> vbuffer;
		BufferData vdata (vertexData, sizeof(vertexData));
		BufferDesc vbufferDesc;
		vbufferDesc.Name = "test";
		vbufferDesc.Size = sizeof(vertexData);
		vbufferDesc.Usage = USAGE_IMMUTABLE;
		vbufferDesc.BindFlags = BIND_VERTEX_BUFFER;
		m_RenderDevice->CreateBuffer(vbufferDesc, &vdata, &vbuffer);

		RefCntAutoPtr<IBuffer> ibuffer;
		BufferData idata (indexData, sizeof(indexData));
		BufferDesc ibufferDesc;
		ibufferDesc.Name = "test2";
		ibufferDesc.Size = sizeof(indexData);
		ibufferDesc.Usage = USAGE_IMMUTABLE;
		ibufferDesc.BindFlags = BIND_INDEX_BUFFER;
		m_RenderDevice->CreateBuffer(ibufferDesc, &idata, &ibuffer);

		m_e2ee = m_DrawChain.CreateCommand(ibuffer, vbuffer, material2, 6);
		m_eee = m_DrawChain.CreateCommand(ibuffer, vbuffer, material1, 6);
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

	std::shared_ptr<EntityDrawCmdHandle> m_eee;
	std::shared_ptr<EntityDrawCmdHandle> m_e2ee;
	EntityDrawCmdList m_DrawChain;
	RefCntAutoPtr<IRenderDevice> m_RenderDevice;
	RefCntAutoPtr<IDeviceContext> m_DeviceContext;
	RefCntAutoPtr<ISwapChain> m_SwapChain;
	RefCntAutoPtr<IPipelineState> m_PipelineState;
	RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
	std::shared_ptr<MaterialInstance> material1;
	std::shared_ptr<MaterialInstance> material2;
	RefCntAutoPtr<IBuffer> buffer1;
	RefCntAutoPtr<IBuffer> buffer2;
};
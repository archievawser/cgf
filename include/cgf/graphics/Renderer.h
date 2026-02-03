#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "core/Common.h"
#include "core/AssetLibrary.h"
#include "core/Game.h"
#include "core/Events.h"

#include "graphics/Diligent.h"
#include "graphics/Shader.h"
#include "graphics/Material.h"


/**
 * @brief Contains the information depended on by a majority of simple draw calls
 */
struct DrawCmdInfo
{
	DrawCmdInfo(
		RefCntAutoPtr<IBuffer> indexBuffer,
		RefCntAutoPtr<IBuffer> vertexBuffer,
		SharedPtr<MaterialInstance> material,
		int indexCount);

	int IndexCount;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	SharedPtr<MaterialInstance> DrawMaterial;
};


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
class RenderGraphBuilder
{
public:
	RefCntAutoPtr<ITexture> MakeTexture2D(
		std::string name, 
		unsigned int width, 
		unsigned int height, 
		TEXTURE_FORMAT format,
		TextureData* data = nullptr);

	void QueuePass(SharedPtr<RenderPass> pass);

	FORCEINLINE const std::vector<DrawCmdInfo>& GetDrawCmdInfoList()
	{
		return m_DrawCmdInfoList;
	}

private:
	std::vector<DrawCmdInfo> m_DrawCmdInfoList;
	std::vector<RenderPass> m_Passes;
};


/**
 * @brief Manages a majority of graphical operations performed by the engine, especially the
 * frame-by-frame visualization of scenes.
 */
class Renderer
{
public:
	Renderer(Window* window);

	void Render();

	void Execute(DrawCmdInfo& cmd);

	bool InitializeDiligent(Window* window);

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

	Event<RenderGraphBuilder&> OnBuildingRenderGraph;

private:
	RefCntAutoPtr<IRenderDevice> m_RenderDevice;
	RefCntAutoPtr<IDeviceContext> m_DeviceContext;
	RefCntAutoPtr<ISwapChain> m_SwapChain;
	RefCntAutoPtr<IPipelineState> m_PipelineState;
	RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
};
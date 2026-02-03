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

	void Execute();

	void AddPass(std::shared_ptr<RenderPass> pass);

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
		SharedPtr<MaterialInstance> material,
		int indexCount);

	int IndexCount;
	bool Valid = true;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	SharedPtr<MaterialInstance> DrawMaterial;
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
	friend struct EntityDrawCmdHandle;

public:
	EntityDrawCmdList() = default;

	SharedPtr<EntityDrawCmdHandle> CreateCommand(
		RefCntAutoPtr<IBuffer> indexBuffer,
		RefCntAutoPtr<IBuffer> vertexBuffer,
		SharedPtr<MaterialInstance> material,
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
	Renderer(Window* window);

	void Render();

	void Execute(EntityDrawCmd& cmd);

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
	
	EntityDrawCmdList DrawChain;

private:
	RefCntAutoPtr<IRenderDevice> m_RenderDevice;
	RefCntAutoPtr<IDeviceContext> m_DeviceContext;
	RefCntAutoPtr<ISwapChain> m_SwapChain;
	RefCntAutoPtr<IPipelineState> m_PipelineState;
	RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
};
#pragma once

#include <memory>
#include <vector>
#include <queue>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"

#include "core/AssetLibrary.h"
#include "core/Architecture.h"
#include "core/Events.h"
#include "core/Window.h"
#include "core/Common.h"
#include "core/Memory.h"
#include "core/Game.h"

#include "graphics/Diligent.h"
#include "graphics/Material.h"
#include "graphics/Shader.h"


/**
 * @brief 
 */
class MeshDrawFactoryComponent : public ActorComponent
{
public:
	MeshDrawFactoryComponent(
		RefCntAutoPtr<IBuffer> indexBuffer,
		RefCntAutoPtr<IBuffer> vertexBuffer,
		SharedPtr<MaterialInstance> material,
		int indexCount);

	typedef ActorComponent Super;
	int IndexCount;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	SharedPtr<MaterialInstance> DrawMaterial;
};


class TestDrawActor : public Actor
{
	typedef Actor Super;

public:
	TestDrawActor(RefCntAutoPtr<IBuffer> indexBuffer,
		RefCntAutoPtr<IBuffer> vertexBuffer,
		SharedPtr<MaterialInstance> material,
		int indexCount)
	{
		DrawInfo = SharedPtr<MeshDrawFactoryComponent>::Create(indexBuffer, vertexBuffer, material, indexCount);
	}

	void Tick(double dT) override
	{
		Super::Tick(dT);
	}

	SharedPtr<MeshDrawFactoryComponent> DrawInfo;
};


struct MeshDrawInfo
{
	int IndexCount;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	SharedPtr<MaterialInstance> DrawMaterial;
};


struct ComponentRenderProxy
{

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

private:
	std::vector<RenderPass> m_Passes;
};


/**
 * @brief Manages a majority of graphical operations performed by the engine, especially the
 * frame-by-frame visualization of scenes.
 */
class Renderer
{
public:
	void Render();
	
	void Execute();
};
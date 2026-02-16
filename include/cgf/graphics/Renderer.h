#pragma once

#include <memory>
#include <vector>
#include <queue>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"

#include "core/AssetLibrary.h"
#include "core/Events.h"
#include "core/Window.h"
#include "core/Common.h"
#include "core/Memory.h"
#include "core/Game.h"

#include "graphics/Diligent.h"
#include "graphics/Material.h"
#include "graphics/Shader.h"
#include "graphics/Mesh.h"


struct PrimitiveRenderState
{
	glm::mat4 Transform;
	SharedPtr<BaseMesh> Mesh;
	SharedPtr<MaterialInstance> DrawMaterial;
	bool Translucent;
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
	
	void Draw(Pool<PrimitiveRenderState>& meshDrawList);

	void Draw(SharedPtr<Scene> scene);
	
	void Execute();
};
#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "core/Common.h"


namespace RDG
{

/**
 * @brief The render graph of a scene organizes & optimizes the execution of render passes involved in rendering each frame
 */
class RenderGraph
{
public:
	RenderGraph() = default;

	void Execute();

private:
	std::vector<RenderPass> m_Passes;
};


/**
 * @brief The RenderGraphFactory is used to declare the specification of render passes and construct a RenderGraph accordingly
 */
class RenderGraphFactory
{
public:
	RenderGraphFactory();

	std::shared_ptr<RenderGraph> Compile();

	FORCEINLINE void AddPass(RenderPass* pass)
	{
		m_Passes.emplace(pass);
	}

private:
	std::queue<RenderPass> m_Passes;
};


/**
 * @brief Render passes are the individual steps executed to render each frame
 * 
 */
class RenderPass
{
public:
	RenderPass() = default;
	
	virtual ~RenderPass() = default;

	virtual void OnExecution() = 0;
};	

}
#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "core/Common.h"
#include "graphics/Diligent.h"


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


Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_RenderDevice;
Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_DeviceContext;
Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_SwapChain;
Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_PipelineState;


/**
 * @brief Render passes are the individual steps executed to render each frame
 */
struct RenderPass
{
	virtual void Run(Diligent::IRenderDevice *renderDevice, Diligent::IDeviceContext* deviceContext) = 0;

	Diligent::ITextureView* RenderTargetView = nullptr;
	Diligent::ITextureView* DepthStencilView = nullptr;
};


struct GBufferPass : public RenderPass
{
	void Run(Diligent::IRenderDevice *renderDevice, Diligent::IDeviceContext *deviceContext) override
	{
		
	}
};


/**
 * @brief The render graph of a scene organizes & optimizes the execution of render passes involved in rendering each frame
 */
class RenderGraph
{
public:
	RenderGraph();

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
	RenderGraphFactory() = default;

	void Compile(RenderGraph* result);

	Diligent::ITexture* DeclareTexture2D(
		std::string name, 
		unsigned int width, 
		unsigned int height, 
		Diligent::TEXTURE_FORMAT format,
		Diligent::TextureData* data = nullptr);

	FORCEINLINE void AddPass(RenderPass pass)
	{
		m_Passes.push_back(pass);
	}

private:
	std::vector<RenderPass> m_Passes;
};


void Test()
{
	RenderGraphFactory rdgBuilder;

	auto texture = rdgBuilder.DeclareTexture2D("Color", 1920, 1080, Diligent::TEX_FORMAT_RGBA8_SINT);

	RenderGraph renderer;
	rdgBuilder.Compile(&renderer);
}
#include <chrono>

#include "glm/gtc/matrix_transform.hpp"

#include "core/Window.h"

#include "graphics/Renderer.h"
#include "graphics/Context.h"
#include "graphics/Material.h"
#include "graphics/Diligent.h"


RefCntAutoPtr<ITexture> RenderGraphBuilder::MakeTexture2D(
	std::string name, 
	unsigned int width, 
	unsigned int height, 
	TEXTURE_FORMAT format, 
	TextureData *data)
{
	TextureDesc textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Name = name.c_str();
	textureDesc.Format = format;
	textureDesc.Type = RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D;
	textureDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_DEPTH_STENCIL | BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;

	RefCntAutoPtr<ITexture> output;
	Game->GetGraphicsContext()->GetRenderDevice()->CreateTexture(textureDesc, data, &output);

	return output;
}


void RenderGraphBuilder::QueuePass(SharedPtr<RenderPass> pass)
{
	
}


void Renderer::Render()
{
	const float ClearColor[] = { 0.f, 0.f, 0.f, 1.0f };

	GraphicsContext* ctx = Game->GetGraphicsContext();

	auto *pRTV = ctx->GetSwapChain()->GetCurrentBackBufferRTV();
	auto *pDSV = ctx->GetSwapChain()->GetDepthBufferDSV();
	ctx->GetDeviceContext()->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	ctx->GetDeviceContext()->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	ctx->GetDeviceContext()->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	Draw(Game->GetCurrentScene());

	ctx->GetSwapChain()->Present();
}


void Renderer::Draw(std::vector<MeshDrawInfo>& meshDrawList)
{
	for(MeshDrawInfo& info : meshDrawList)
	{
		IBuffer* vbuffers[] = { info.VertexBuffer };

		GraphicsContext* ctx = Game->GetGraphicsContext();

		ctx->GetDeviceContext()->SetVertexBuffers(0, 1, vbuffers, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
		ctx->GetDeviceContext()->SetIndexBuffer(info.IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		
		ctx->UsePipeline(info.DrawMaterial->GetBaseMaterial()->GetPipelineState());

		ctx->GetDeviceContext()->CommitShaderResources(info.DrawMaterial->GetResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		DrawIndexedAttribs drawAttrs;
		drawAttrs.NumIndices = info.IndexCount;
		drawAttrs.IndexType = VT_UINT32;
		ctx->GetDeviceContext()->DrawIndexed(drawAttrs);
	}
}


void Renderer::Draw(SharedPtr<Scene> scene)
{
	Draw(scene->MeshDrawList);
}
#include "graphics/Renderer.h"


RefCntAutoPtr<ITexture> RenderGraph::MakeTexture2D(
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
	Singleton<Renderer>::Get()->GetRenderDevice()->CreateTexture(textureDesc, data, &output);

	return output;
}


void Renderer::Render()
{
	const float ClearColor[] = {0.f, 0.f, 0.f, 1.0f};

	auto *pRTV = m_SwapChain->GetCurrentBackBufferRTV();
	auto *pDSV = m_SwapChain->GetDepthBufferDSV();
	m_DeviceContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	m_DeviceContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	m_DeviceContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	m_DeviceContext->SetPipelineState(material->GetPipelineState());

	m_DeviceContext->CommitShaderResources(material->GetResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	DrawAttribs drawAttrs;
	drawAttrs.NumVertices = 6;
	m_DeviceContext->Draw(drawAttrs);

	m_SwapChain->Present();
}
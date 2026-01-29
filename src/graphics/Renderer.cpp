#include "graphics/Renderer.h"
#include <chrono>
#include "glm/gtc/matrix_transform.hpp"
#include "graphics/Material.h"


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
	const float ClearColor[] = { 0.f, 0.f, 0.f, 1.0f };

	auto *pRTV = m_SwapChain->GetCurrentBackBufferRTV();
	auto *pDSV = m_SwapChain->GetDepthBufferDSV();
	m_DeviceContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	m_DeviceContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	m_DeviceContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	m_DrawChain.Execute();

	m_SwapChain->Present();
}


void Renderer::Execute(EntityDrawCmd& cmd)
{
	IBuffer* vbuffers[] { cmd.VertexBuffer };

	m_DeviceContext->SetVertexBuffers(0, 1, vbuffers, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
	m_DeviceContext->SetIndexBuffer(cmd.IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	
	UsePipeline(cmd.DrawMaterial->GetBaseMaterial()->GetPipelineState());
	
	glm::mat4 proj = glm::perspectiveFov(70.f, 1920.f, 1080.f, 0.1f, 1000.f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -10));
	glm::mat4 model = glm::mat4x4(1.0f);

	glm::mat4x4 mvp = proj * view * model;

	m_DeviceContext->UpdateBuffer(buffer, 0, sizeof(glm::mat4x4), glm::value_ptr(mvp), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	m_DeviceContext->CommitShaderResources(cmd.DrawMaterial->GetResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	DrawIndexedAttribs drawAttrs;
	drawAttrs.NumIndices = 6;
	drawAttrs.IndexType = VT_UINT32;
	m_DeviceContext->DrawIndexed(drawAttrs);
}

std::shared_ptr<EntityDrawCmdHandle> DrawCommandChain::PushCommand(
	RefCntAutoPtr<IBuffer> indexBuffer,
	RefCntAutoPtr<IBuffer> vertexBuffer,
	std::shared_ptr<MaterialInstance> material,
	int indexCount)
{
	m_CommandList.push_back(EntityDrawCmd(indexBuffer, vertexBuffer, material, indexCount));

	return std::make_shared<EntityDrawCmdHandle>(&m_CommandList.back());
}


void DrawCommandChain::Execute()
{
	for (int i = 0; i < m_CommandList.size(); i++)
	{
		EntityDrawCmd& cmd = m_CommandList[i];

		if (cmd.Valid)
		{
			Singleton<Renderer>::Get()->Execute(cmd);

			return;
		}
	}
}


EntityDrawCmdHandle::EntityDrawCmdHandle(EntityDrawCmd *cmd)
	: Command(cmd)
{

}


EntityDrawCmdHandle::~EntityDrawCmdHandle()
{
	Command->Valid = false;
}

EntityDrawCmd::EntityDrawCmd(
	RefCntAutoPtr<IBuffer> indexBuffer,
	RefCntAutoPtr<IBuffer> vertexBuffer,
	std::shared_ptr<MaterialInstance> material,
	int indexCount)
	: IndexBuffer(indexBuffer), VertexBuffer(vertexBuffer), DrawMaterial(material), IndexCount(indexCount)
{

}
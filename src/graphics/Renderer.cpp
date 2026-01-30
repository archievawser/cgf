#include "graphics/Renderer.h"
#include <chrono>
#include "glm/gtc/matrix_transform.hpp"
#include "graphics/Material.h"
#include "core/Window.h"


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
	Game->GetRenderer()->GetRenderDevice()->CreateTexture(textureDesc, data, &output);

	return output;
}


void RenderGraph::Execute()
{
	for (std::shared_ptr<RenderPass> pass : m_Passes)
	{
		pass->Run(this);
	}
}


void RenderGraph::AddPass(std::shared_ptr<RenderPass> pass)
{
	m_Passes.push_back(pass);
}


Renderer::Renderer(Window* window)
{
	InitializeDiligent(window);
}


void Renderer::Render()
{
	const float ClearColor[] = { 0.f, 0.f, 0.f, 1.0f };

	auto *pRTV = m_SwapChain->GetCurrentBackBufferRTV();
	auto *pDSV = m_SwapChain->GetDepthBufferDSV();
	m_DeviceContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	m_DeviceContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	m_DeviceContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	DrawChain.Execute();

	m_SwapChain->Present();
}


void Renderer::Execute(EntityDrawCmd& cmd)
{
	IBuffer* vbuffers[] { cmd.VertexBuffer };

	m_DeviceContext->SetVertexBuffers(0, 1, vbuffers, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
	m_DeviceContext->SetIndexBuffer(cmd.IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	
	UsePipeline(cmd.DrawMaterial->GetBaseMaterial()->GetPipelineState());

	m_DeviceContext->CommitShaderResources(cmd.DrawMaterial->GetResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	DrawIndexedAttribs drawAttrs;
	drawAttrs.NumIndices = cmd.IndexCount;
	drawAttrs.IndexType = VT_UINT32;
	m_DeviceContext->DrawIndexed(drawAttrs);
}


std::shared_ptr<EntityDrawCmdHandle> EntityDrawCmdList::CreateCommand(
	RefCntAutoPtr<IBuffer> indexBuffer,
	RefCntAutoPtr<IBuffer> vertexBuffer,
	std::shared_ptr<MaterialInstance> material,
	int indexCount)
{
	m_CmdList.push_back(EntityDrawCmd(indexBuffer, vertexBuffer, material, indexCount));

	return std::make_shared<EntityDrawCmdHandle>(m_CmdList.size() - 1, this);
}


void EntityDrawCmdList::Execute()
{
	for (EntityDrawCmd& cmd : m_CmdList)
	{
		if(cmd.Valid)
		{
			Game->GetRenderer()->Execute(cmd);
		}
	}
}


EntityDrawCmdHandle::EntityDrawCmdHandle(int cmdIndex, EntityDrawCmdList *chain)
	: CmdIndex(cmdIndex), Chain(chain)
{

}


EntityDrawCmdHandle::~EntityDrawCmdHandle()
{
	Chain->m_CmdList[CmdIndex].Valid = false;
}


EntityDrawCmd::EntityDrawCmd(
	RefCntAutoPtr<IBuffer> indexBuffer,
	RefCntAutoPtr<IBuffer> vertexBuffer,
	std::shared_ptr<MaterialInstance> material,
	int indexCount)
	: IndexBuffer(indexBuffer), VertexBuffer(vertexBuffer), DrawMaterial(material), IndexCount(indexCount)
{

}


bool Renderer::InitializeDiligent(Window* window)
{
	auto Window = Game->GetWindow()->GetNativeWindowHandle();

	SwapChainDesc SCDesc;

	switch (m_DeviceType)
	{
#if D3D11_SUPPORTED
	case RENDER_DEVICE_TYPE_D3D11:
	{
		EngineD3D11CreateInfo EngineCI;

		auto *GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
		auto *pFactoryD3D11 = GetEngineFactoryD3D11();
		pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_RenderDevice, &m_DeviceContext);
		pFactoryD3D11->CreateSwapChainD3D11(m_RenderDevice, m_DeviceContext, SCDesc, FullScreenModeDesc{}, Window, &m_SwapChain);
	}
	break;
#endif

#if D3D12_SUPPORTED
	case RENDER_DEVICE_TYPE_D3D12:
	{
		auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();

		EngineD3D12CreateInfo EngineCI;
		auto *pFactoryD3D12 = GetEngineFactoryD3D12();
		pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_RenderDevice, &m_DeviceContext);
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
		EngineCI.Window = Window;
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

		if (!m_SwapChain)
		{
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

	if (m_RenderDevice == nullptr || m_SwapChain == nullptr)
		return false;

	return true;
}
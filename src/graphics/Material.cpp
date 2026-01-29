#include "graphics/Material.h"

#include "core/Common.h"
#include "graphics/Renderer.h"


Material::Material(std::shared_ptr<Shader> vs, std::shared_ptr<Shader> ps)
	: m_PixelShader(ps), m_VertexShader(vs)
{
	auto swapChain = Singleton<Renderer>::Get()->GetSwapChain();
	std::fill_n(m_RenderTargetFormats, _countof(m_RenderTargetFormats), swapChain->GetCurrentBackBufferRTV()->GetDesc().Format);
	m_DepthStencilFormat = swapChain->GetDepthBufferDSV()->GetDesc().Format;

	m_PipelineState = BuildPipeline();
	m_PipelineState->CreateShaderResourceBinding(&m_ResourceBinding);
}


RefCntAutoPtr<IPipelineState> Material::BuildPipeline()
{
	GraphicsPipelineStateCreateInfo PSOCreateInfo;
	PSOCreateInfo.PSODesc.Name = "Material Pipeline";
	PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
	PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;
	PSOCreateInfo.GraphicsPipeline.NumRenderTargets = m_RenderTargetCount;
	
	for(int i = 0; i < m_RenderTargetCount; i++)
	{
		PSOCreateInfo.GraphicsPipeline.RTVFormats[i] = m_RenderTargetFormats[i];
	}

	PSOCreateInfo.GraphicsPipeline.DSVFormat = m_DepthStencilFormat;
	PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = m_PrimitiveType;
	PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = m_CullMode;
	PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = m_UseDepth;
	PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = m_VertexLayout.data();
	PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = m_VertexLayout.size();
	PSOCreateInfo.pVS = m_VertexShader->GetHandle();
	PSOCreateInfo.pPS = m_PixelShader->GetHandle();

	RefCntAutoPtr<IPipelineState> pipelineState;
	Singleton<Renderer>::Get()->GetRenderDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &pipelineState);
	return pipelineState;
}
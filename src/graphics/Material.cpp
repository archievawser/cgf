#include "graphics/Material.h"
#include "core/Common.h"
#include "graphics/Renderer.h"


Material::Material(std::shared_ptr<Shader> vs, std::shared_ptr<Shader> ps)
	: m_PixelShader(ps), m_VertexShader(vs)
{
	auto swapChain = Game->GetRenderer()->GetSwapChain();
	std::fill_n(m_RenderTargetFormats, _countof(m_RenderTargetFormats), swapChain->GetCurrentBackBufferRTV()->GetDesc().Format);
	m_DepthStencilFormat = swapChain->GetDepthBufferDSV()->GetDesc().Format;

	m_VertexLayout.push_back(LayoutElement("POSITION", 0, 0, 3, VT_FLOAT32));
	m_VertexLayout.push_back(LayoutElement("NORMAL", 0, 0, 3, VT_FLOAT32));
	m_VertexLayout.push_back(LayoutElement("TEXCOORD", 0, 0, 2, VT_FLOAT32));

	EnsurePipelineValidity();
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
	PSOCreateInfo.GraphicsPipeline.RasterizerDesc.FillMode = m_FillMode;
	PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = m_CullMode;
	PSOCreateInfo.GraphicsPipeline.RasterizerDesc.FrontCounterClockwise = true;
	PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = m_UseDepth;
	PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = m_VertexLayout.data();
	PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = m_VertexLayout.size();
	PSOCreateInfo.pVS = m_VertexShader->GetHandle();
	PSOCreateInfo.pPS = m_PixelShader->GetHandle();

	RefCntAutoPtr<IPipelineState> pipelineState;
	Game->GetRenderer()->GetRenderDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &pipelineState);
	return pipelineState;
}


MaterialInstance::MaterialInstance(SharedPtr<Material> material)
	: m_BaseMaterial(material)
{
	m_BaseMaterial->GetPipelineState()->CreateShaderResourceBinding(&m_ResourceBinding, true);
}
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "core/Common.h"
#include "core/Game.h"


Material::Material(std::shared_ptr<Shader> vs, std::shared_ptr<Shader> ps, MaterialDomain domain)
	: m_PixelShader(ps), m_VertexShader(vs), m_Domain(domain)
{
	auto swapChain = Game->GetGraphicsContext()->GetSwapChain();
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

	BlendStateDesc &BSDesc = PSOCreateInfo.GraphicsPipeline.BlendDesc;

	// Optional: enable independent blending if you have multiple simultaneous RTs
	BSDesc.IndependentBlendEnable = True;

	// Configure blending for render target 0
	auto &RT0 = BSDesc.RenderTargets[0];
	RT0.BlendEnable        = True;
	RT0.RenderTargetWriteMask = COLOR_MASK_ALL;
	RT0.SrcBlend           = BLEND_FACTOR_SRC_ALPHA;
	RT0.DestBlend          = BLEND_FACTOR_INV_SRC_ALPHA;
	RT0.BlendOp            = BLEND_OPERATION_ADD;

	// Optionally blend alpha similarly
	RT0.SrcBlendAlpha      = BLEND_FACTOR_SRC_ALPHA;
	RT0.DestBlendAlpha     = BLEND_FACTOR_INV_SRC_ALPHA;
	RT0.BlendOpAlpha       = BLEND_OPERATION_ADD;
	
	for(int i = 0; i < m_RenderTargetCount; i++)
	{
		PSOCreateInfo.GraphicsPipeline.RTVFormats[i] = m_RenderTargetFormats[i];
	}

	SamplerDesc SamLinearClampDesc;
	SamLinearClampDesc.MinFilter = FILTER_TYPE_POINT;
	SamLinearClampDesc.MagFilter = FILTER_TYPE_POINT;
	SamLinearClampDesc.MipFilter = FILTER_TYPE_POINT;
	SamLinearClampDesc.AddressU  = TEXTURE_ADDRESS_CLAMP;
	SamLinearClampDesc.AddressV  = TEXTURE_ADDRESS_CLAMP;
	SamLinearClampDesc.AddressW  = TEXTURE_ADDRESS_CLAMP;

	ImmutableSamplerDesc ImtblSamplers[] =
	{
		{SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
	};

	PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
	PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

	PSOCreateInfo.GraphicsPipeline.DSVFormat = m_DepthStencilFormat;
	PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = m_PrimitiveType;
	PSOCreateInfo.GraphicsPipeline.RasterizerDesc.FillMode = m_FillMode;
	PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = m_CullMode;
	PSOCreateInfo.GraphicsPipeline.RasterizerDesc.FrontCounterClockwise = true;
	PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = m_UseDepth;
	
	if(m_Domain == MaterialDomain::Translucent)
	{
		PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;
	}

	PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = m_VertexLayout.data();
	PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = m_VertexLayout.size();
	PSOCreateInfo.pVS = m_VertexShader->GetHandle();
	PSOCreateInfo.pPS = m_PixelShader->GetHandle();

	RefCntAutoPtr<IPipelineState> pipelineState;
	Game->GetGraphicsContext()->GetRenderDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &pipelineState);
	return pipelineState;
}


MaterialInstance::MaterialInstance(SharedPtr<Material> material)
	: m_BaseMaterial(material)
{
	m_BaseMaterial->GetPipelineState()->CreateShaderResourceBinding(&m_ResourceBinding, true);

	ShaderCommon = CreateVertexVariableBinding<ShaderCommonData>("ShaderCommon");
}
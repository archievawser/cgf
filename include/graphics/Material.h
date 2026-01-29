#pragma once

#include <memory>

#include "graphics/Diligent.h"
#include "graphics/Shader.h"

/**
 * @brief A Material object defines the manner and context in which related draw calls are performed
 * @image cold.png
 */
class Material
{
public:
	Material(std::shared_ptr<Shader> vs, std::shared_ptr<Shader> ps);

	FORCEINLINE IShaderResourceVariable* GetFragmentVariable(const char* name)
	{
		return m_ResourceBinding->GetVariableByName(SHADER_TYPE_PIXEL, name);
	}

	FORCEINLINE RefCntAutoPtr<IShaderResourceBinding> GetResourceBinding() const
	{
		return m_ResourceBinding;
	}

	FORCEINLINE RefCntAutoPtr<IPipelineState> GetPipelineState()
	{
		if(!m_PipelineValid)
		{
			m_PipelineState = BuildPipeline();
		}
		
		return m_PipelineState;
	}

	FORCEINLINE void SetVertexLayout(std::vector<LayoutElement>& layout)
	{
		m_VertexLayout = layout;

		OnPipelineMutated();
	}

	FORCEINLINE void SetCullMode(CULL_MODE mode)
	{
		m_CullMode = mode;

		OnPipelineMutated();
	}

protected:
	RefCntAutoPtr<IPipelineState> BuildPipeline();
	
	FORCEINLINE void OnPipelineMutated()
	{
		m_PipelineValid = false;
	}

private:
	std::vector<LayoutElement> m_VertexLayout;
	RefCntAutoPtr<IPipelineState> m_PipelineState;
	RefCntAutoPtr<IShaderResourceBinding> m_ResourceBinding;
	TEXTURE_FORMAT m_RenderTargetFormats[8];
	int m_RenderTargetCount = 1;
	TEXTURE_FORMAT m_DepthStencilFormat = TEX_FORMAT_D32_FLOAT;
	PRIMITIVE_TOPOLOGY m_PrimitiveType = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	CULL_MODE m_CullMode = CULL_MODE_NONE;
	bool m_UseDepth = false;
	std::shared_ptr<Shader> m_PixelShader = nullptr;	
	std::shared_ptr<Shader> m_VertexShader = nullptr;	
	bool m_PipelineValid = false;
};
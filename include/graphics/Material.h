#pragma once

#include <memory>

#include "graphics/Diligent.h"
#include "graphics/Shader.h"


class MaterialInstance;


/**
 * @brief A Material object defines the manner and context in which related draw calls are performed
 */
class Material
{
public:
	Material(std::shared_ptr<Shader> vs, std::shared_ptr<Shader> ps);

	/**
	 * @return The material's current graphics pipeline
	 */
	FORCEINLINE const RefCntAutoPtr<IPipelineState>& GetPipelineState()
	{
		EnsurePipelineValidity();

		return m_PipelineState;
	}

	/**
	 * @brief Modifies the vertex buffer layout associated with this Material
	 */
	FORCEINLINE void SetVertexLayout(std::vector<LayoutElement>& layout)
	{
		m_VertexLayout = layout;

		InvalidateCurrentPipeline();
	}

	/**
	 * @brief Modifies the faces to be ignored during rendering; one of CULL_MODE_BACK, CULL_MODE_FRONT, and CULL_MODE_NONE
	 */
	FORCEINLINE void SetCullMode(CULL_MODE mode)
	{
		m_CullMode = mode;

		InvalidateCurrentPipeline();
	}

	/**
	 * @brief Modifies the faces to be ignored during rendering; one of CULL_MODE_BACK, CULL_MODE_FRONT, and CULL_MODE_NONE
	 */
	FORCEINLINE void SetFillMode(FILL_MODE mode)
	{
		m_FillMode = mode;

		InvalidateCurrentPipeline();
	}

protected:
	RefCntAutoPtr<IPipelineState> BuildPipeline();

	FORCEINLINE void EnsurePipelineValidity()
	{
		if(!m_PipelineValid)
		{
			m_PipelineState = BuildPipeline();
			m_PipelineValid = true;
		}
	}
	
	FORCEINLINE void InvalidateCurrentPipeline()
	{
		m_PipelineValid = false;
	}

private:
	std::vector<LayoutElement> m_VertexLayout;
	RefCntAutoPtr<IPipelineState> m_PipelineState;
	TEXTURE_FORMAT m_RenderTargetFormats[8];
	int m_RenderTargetCount = 1;
	TEXTURE_FORMAT m_DepthStencilFormat = TEX_FORMAT_D32_FLOAT;
	PRIMITIVE_TOPOLOGY m_PrimitiveType = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	CULL_MODE m_CullMode = CULL_MODE_NONE;
	FILL_MODE m_FillMode = FILL_MODE_SOLID;
	bool m_UseDepth = false;
	std::shared_ptr<Shader> m_PixelShader = nullptr;	
	std::shared_ptr<Shader> m_VertexShader = nullptr;	
	bool m_PipelineValid = false;
};


/**
 * @brief A particular instance of a Material allowing instance-specific resource bindings
 */
class MaterialInstance
{
public:
	MaterialInstance(std::shared_ptr<Material> material);

	/**
	 * @return A handle to a variable within the fragment shader; nullptr if no matching variable was found
	 */
	FORCEINLINE IShaderResourceVariable* GetFragmentShaderVariable(const char *name)
	{
		return m_ResourceBinding->GetVariableByName(SHADER_TYPE_PIXEL, name);
	}

	/**
	 * @return A handle to a variable within the fragment shader; nullptr if no matching variable was found
	 */
	FORCEINLINE IShaderResourceVariable* GetVertexShaderVariable(const char *name)
	{
		return m_ResourceBinding->GetVariableByName(SHADER_TYPE_VERTEX, name);
	}

	/**
	 * @return The IShaderResourceBinding associated with this MaterialInstance
	 */
	FORCEINLINE RefCntAutoPtr<IShaderResourceBinding> GetResourceBinding() const
	{
		return m_ResourceBinding;
	}

	std::shared_ptr<Material> GetBaseMaterial() const 
	{ 
		return m_BaseMaterial; 
	}

private:
	RefCntAutoPtr<IShaderResourceBinding> m_ResourceBinding;
	std::shared_ptr<Material> m_BaseMaterial;
};
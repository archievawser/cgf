#pragma once

#include <memory>

#include "core/Common.h"
#include "core/Memory.h"
#include "core/Game.h"

#include "graphics/Diligent.h"
#include "graphics/Shader.h"


class Renderer;
class MaterialInstance;


/**
 * @brief A Material object defines the manner and context in which related draw calls are performed
 */
class Material : public ManagedObject
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


template<typename T>
class DeviceVarBinding
{
public:
	DeviceVarBinding(IShaderResourceVariable* dest)	
		: m_Destination(dest)
	{
		ShaderResourceDesc desc;
		dest->GetResourceDesc(desc);

		Diligent::BIND_FLAGS target = desc.Type == SHADER_RESOURCE_TYPE_CONSTANT_BUFFER
										? BIND_UNIFORM_BUFFER
										: BIND_SHADER_RESOURCE;

		BufferDesc bufferDesc;
		bufferDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		bufferDesc.Name = (std::string(desc.Name) + "_buffer").c_str();
		bufferDesc.Usage = USAGE_DEFAULT;
		bufferDesc.Size = sizeof(T);
		bufferDesc.BindFlags = target;

		Game->GetRenderer()->GetRenderDevice()->CreateBuffer(bufferDesc, nullptr, &m_Buffer);
	}

	void Set(T& value)
	{
		Game->GetRenderer()->GetDeviceContext()->UpdateBuffer(m_Buffer, 0, sizeof(T), &value, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_Destination->Set(m_Buffer);
	}

private:
	RefCntAutoPtr<IBuffer> m_Buffer;
	RefCntAutoPtr<IShaderResourceVariable> m_Destination;
};


/**
 * @brief A particular instance of a Material allowing instance-specific resource bindings
 */
class MaterialInstance
{
public:
	MaterialInstance(SharedPtr<Material> material);

	template<typename T>
	FORCEINLINE DeviceVarBinding<T> CreateFragmentVariableBinding(const char* name)
	{
		auto v = GetFragmentShaderVariable(name);

		CGF_ENSURE_NOT_NULLPTR(v);
		
		return DeviceVarBinding<T>(v);
	}

	template<typename T>
	FORCEINLINE DeviceVarBinding<T> CreateVertexVariableBinding(const char* name)
	{
		auto v = GetVertexShaderVariable(name);

		CGF_ENSURE_NOT_NULLPTR(v);

		return DeviceVarBinding<T>(v);
	}

	/**
	 * @return A handle to a variable within the fragment shader; nullptr if no matching variable was found
	 */
	FORCEINLINE RefCntAutoPtr<IShaderResourceVariable> GetFragmentShaderVariable(const char *name)
	{
		return RefCntAutoPtr<IShaderResourceVariable>(m_ResourceBinding->GetVariableByName(SHADER_TYPE_PIXEL, name));
	}

	/**
	 * @return A handle to a variable within the fragment shader; nullptr if no matching variable was found
	 */
	FORCEINLINE RefCntAutoPtr<IShaderResourceVariable> GetVertexShaderVariable(const char *name)
	{
		return RefCntAutoPtr<IShaderResourceVariable>(m_ResourceBinding->GetVariableByName(SHADER_TYPE_VERTEX, name));
	}

	/**
	 * @return The IShaderResourceBinding associated with this MaterialInstance
	 */
	FORCEINLINE RefCntAutoPtr<IShaderResourceBinding> GetResourceBinding() const
	{
		return m_ResourceBinding;
	}

	SharedPtr<Material> GetBaseMaterial() const
	{ 
		return m_BaseMaterial; 
	}

private:
	RefCntAutoPtr<IShaderResourceBinding> m_ResourceBinding;
	SharedPtr<Material> m_BaseMaterial;
};
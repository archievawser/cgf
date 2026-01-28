#pragma once

#include <string>
#include <unordered_map>
#include "graphics/Diligent.h"


class Shader 
{
public:
	Shader(std::string name, 
		std::string source, 
		Diligent::SHADER_TYPE type, 
		Diligent::SHADER_SOURCE_LANGUAGE sourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL);

	Shader(std::string name, 
		std::string source, 
		Diligent::SHADER_TYPE type, 
		std::string entry,
		Diligent::SHADER_SOURCE_LANGUAGE sourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL);

	FORCEINLINE Diligent::RefCntAutoPtr<Diligent::IShader> GetHandle()
	{
		return m_Handle;
	}

	FORCEINLINE Diligent::SHADER_TYPE GetShaderType()
	{
		return m_ShaderType;
	}
	
private:
	Diligent::SHADER_TYPE m_ShaderType;
	Diligent::RefCntAutoPtr<Diligent::IShader> m_Handle;
};
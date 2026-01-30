#pragma once

#include <string>
#include <unordered_map>
#include "graphics/Diligent.h"


class Shader 
{
public:
	Shader(std::string name, 
		std::string& source, 
		SHADER_TYPE type, 
		SHADER_SOURCE_LANGUAGE sourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL);

	Shader(std::string name, 
		std::string& source, 
		SHADER_TYPE type, 
		std::string entry,
		SHADER_SOURCE_LANGUAGE sourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL);

	FORCEINLINE RefCntAutoPtr<IShader> GetHandle()
	{
		return m_Handle;
	}

	FORCEINLINE SHADER_TYPE GetShaderType()
	{
		return m_ShaderType;
	}
	
private:
	static std::unordered_map<SHADER_TYPE, const char*> m_ShaderEntryPoints;
	SHADER_TYPE m_ShaderType;
	RefCntAutoPtr<IShader> m_Handle;
};
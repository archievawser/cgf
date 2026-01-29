#include "graphics/Shader.h"

#include "core/Game.h"
#include "Shader.h"


Shader::Shader(std::string name,
	std::string& source,
	SHADER_TYPE type,
	std::string entryPoint,
	SHADER_SOURCE_LANGUAGE sourceLanguage)
	: m_ShaderType(type)
{
	ShaderCreateInfo info;
	info.Desc.Name = name.c_str();
	info.Desc.ShaderType = type;
	info.Desc.UseCombinedTextureSamplers = true;
	info.Source = source.c_str();
	info.EntryPoint = entryPoint.c_str();
	info.SourceLanguage = sourceLanguage;

	RefCntAutoPtr<IDataBlob> data;
	Singleton<Renderer>::Get()->GetRenderDevice()->CreateShader(info, &m_Handle, &data);
}


Shader::Shader(std::string name, std::string& source, SHADER_TYPE type, SHADER_SOURCE_LANGUAGE sourceLanguage)
	: Shader(name, source, type, "Main", sourceLanguage)
{	
	
}
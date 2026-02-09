#include "graphics/Shader.h"

#include "core/Game.h"
#include "graphics/Renderer.h"


std::unordered_map<SHADER_TYPE, const char*> Shader::m_ShaderEntryPoints = {
	{ SHADER_TYPE_PIXEL, "ProcessFragment" },
	{ SHADER_TYPE_VERTEX, "ProcessVertex" },
};


Shader::Shader(std::string name,
			   std::string &source,
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
	info.EntryPoint = m_ShaderEntryPoints[type];
	info.SourceLanguage = sourceLanguage;

	RefCntAutoPtr<IDataBlob> data;
	Game->GetGraphicsContext()->GetRenderDevice()->CreateShader(info, &m_Handle, &data);
}


Shader::Shader(std::string name, std::string& source, SHADER_TYPE type, SHADER_SOURCE_LANGUAGE sourceLanguage)
	: Shader(name, source, type, "Main", sourceLanguage)
{	
	
}
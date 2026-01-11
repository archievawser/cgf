#include "graphics/Shader.h"

#include "core/Game.h"


Shader::Shader(std::string name,
	std::string source,
	Diligent::SHADER_TYPE type,
	Diligent::SHADER_SOURCE_LANGUAGE sourceLanguage,
	std::string entryPoint)
{
	Diligent::ShaderCreateInfo info;
	info.Desc.Name = name.c_str();
	info.Desc.ShaderType = type;
	info.Desc.UseCombinedTextureSamplers = true;
	info.Source = source.c_str();

	Diligent::IDataBlob* data = nullptr;
	Game::Instance->RenderDevice->CreateShader(info, &m_Handle, &data);
}
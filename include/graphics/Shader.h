#pragma once

#include <string>
#include "graphics/DiligentCommon.h"


class Shader 
{
public:
	Shader(std::string name, 
		std::string source, 
		Diligent::SHADER_TYPE type, 
		Diligent::SHADER_SOURCE_LANGUAGE sourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL,
		std::string entryPoint = "main");

private:
	Diligent::RefCntAutoPtr<Diligent::IShader> m_Handle;
};
#include "graphics/Texture.h"

#include "core/Game.h"


Texture2D::Texture2D(std::string name, 
	unsigned int width, 
	unsigned int height, 
	TEXTURE_FORMAT format,
	const void* data,
	int dataStride)
{
	TextureDesc textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Name = name.c_str();
	textureDesc.Format = format;
	textureDesc.Type = RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D;
	textureDesc.BindFlags = BIND_SHADER_RESOURCE;

	TextureSubResData resData;
	resData.pData = data;
	resData.Stride = dataStride * width;
	resData.DepthStride = 0;

	TextureData texData;
	texData.pSubResources = &resData;
	texData.NumSubresources = 1;

	Game->GetGraphicsContext()->GetRenderDevice()->CreateTexture(textureDesc, &texData, &m_Handle);
}
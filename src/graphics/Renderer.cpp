#include "graphics/Renderer.h"


void RenderGraph::Execute()
{
	for(RenderPass& pass : m_Passes)
	{
		pass.Run(m_RenderDevice, m_DeviceContext);
	}


}
Diligent::ITexture *RenderGraphFactory::DeclareTexture2D(
	std::string name, 
	unsigned int width, 
	unsigned int height, 
	Diligent::TEXTURE_FORMAT format, 
	Diligent::TextureData *data)
{
	Diligent::TextureDesc textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Name = name.c_str();
	textureDesc.Format = format;
	textureDesc.Type = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D;
	textureDesc.BindFlags =
		Diligent::BIND_UNORDERED_ACCESS | Diligent::BIND_DEPTH_STENCIL | Diligent::BIND_RENDER_TARGET | Diligent::BIND_FLAGS::BIND_SHADER_RESOURCE;

	Diligent::ITexture *output;
	m_RenderDevice->CreateTexture(textureDesc, data, &output);

	return output;
}
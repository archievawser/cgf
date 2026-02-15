#pragma once

#include "core/AssetLibrary.h"

#include "graphics/Diligent.h"

#include "stb/stb_image.h"


class Texture2D
{
public:
	Texture2D(std::string name, 
		unsigned int width, 
		unsigned int height, 
		TEXTURE_FORMAT format,
		const void* data,
		int dataStride);

	FORCEINLINE RefCntAutoPtr<ITexture> GetHandle()
	{
		return m_Handle;
	}

	FORCEINLINE int GetWidth() const
	{
		return m_Width;
	}

	FORCEINLINE int GetHeight() const
	{
		return m_Height;
	}

private:
	int m_Width;
	int m_Height;
	RefCntAutoPtr<ITexture> m_Handle;
};


template<>
inline SharedPtr<Texture2D> AssetLibrary::Load(std::string textureName)
{
	char* rawSource;
	int rawSourceLength;
	bool success = m_AssetDataLoader.Load(textureName.c_str(), &rawSource, &rawSourceLength);

	int x, y, channels;
	char* pixelData;

	cgfb::CgfbMemoryReader reader (rawSource);
	reader.Read(&x);
	reader.Read(&y);
	reader.Read(&channels);

	constexpr int bytesPerChannel = sizeof(char);
	const int pixelDataSize = x * y * channels * bytesPerChannel;
	pixelData = new char[pixelDataSize];
	reader.Read(pixelData, pixelDataSize);

	
	CGF_INFO(channels);


	Texture2D* newTexture = new Texture2D(textureName,
		x,
		y,
		TEX_FORMAT_RGBA8_TYPELESS,
		pixelData,
		channels * bytesPerChannel);

	return SharedPtr<Texture2D>(newTexture);
}
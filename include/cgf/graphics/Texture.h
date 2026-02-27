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
	cgfb::CgfbBlock block;
	m_AssetFile.ReadBlock(textureName.c_str(), block);

	int x, y;
	const int channels = 4;
	std::string marker;
	char* pixelData = nullptr;

	cgfb::CgfbMemoryReader reader ( std::move(block) );
	reader.Read(&marker);
	reader.Read(&x);
	reader.Read(&y);

	constexpr int bytesPerChannel = sizeof(char);
	const int pixelDataSize = x * y * channels * bytesPerChannel;
	pixelData = new char[pixelDataSize];
	reader.Read(pixelData, pixelDataSize);

	Texture2D* newTexture = new Texture2D(textureName,
		x,
		y,
		TEX_FORMAT_RGBA8_UNORM_SRGB,
		pixelData,
		channels * bytesPerChannel);

	return SharedPtr<Texture2D>(newTexture);
}
#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "graphics/Shader.h"
#include "graphics/Material.h"

#include "buildtool/Assets.h"


/**
 * @brief Provides access to assets included in a project
 **/
class AssetLibrary
{
public:
	AssetLibrary(const char* projectFilePath);

	/**
	 * @tparam AssetType 
	 * @param assetName The name of the asset as registered in the project descriptor; not its file path
	 * @return AssetType The requested asset
	 */
	template<typename AssetType>
	AssetType Get(std::string assetName);

private:
	AssetDataLoader m_AssetDataLoader;
	char* m_AssetData;
	int m_AssetDataSize;
};


template <typename AssetType>
inline AssetType AssetLibrary::Get(std::string assetName)
{
	throw "Asset type not recognized";
}


template<>
inline Material* AssetLibrary::Get(std::string materialName)
{
	char* rawSource;
	int rawSourceLength;
	m_AssetDataLoader.Load(materialName.c_str(), &rawSource, &rawSourceLength);

	std::string source (rawSource, rawSourceLength);

	auto fs = std::make_shared<Shader>(materialName, source, SHADER_TYPE_PIXEL);
	auto vs = std::make_shared<Shader>(materialName, source, SHADER_TYPE_VERTEX);

	return new Material(vs, fs);
}
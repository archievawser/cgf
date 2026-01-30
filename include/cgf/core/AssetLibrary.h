#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "graphics/Shader.h"


/**
 * @brief Provides access to assets included in a project
 **/
class AssetLibrary
{
public:
	AssetLibrary() = default;

	/**
	 * @tparam AssetType 
	 * @param assetName The name of the asset as registered in the project descriptor; not its file path
	 * @return AssetType The requested asset
	 */
	template<typename AssetType>
	AssetType Get(std::string assetName);

private:
	std::unordered_map<std::string, void*> m_Assets;
};


template <typename AssetType>
inline AssetType AssetLibrary::Get(std::string assetName)
{
	throw "Asset type not recognized";
}


template<>
inline Shader* AssetLibrary::Get(std::string sourcePath)
{
	std::ifstream file ("shaders/" + sourcePath);
	std::string source = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	
	return new Shader(sourcePath, source, sourcePath[0] == 'f' ? SHADER_TYPE_PIXEL : SHADER_TYPE_VERTEX);
}
#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "core/Memory.h"
#include "graphics/Shader.h"
#include "graphics/Material.h"

#include "buildtool/Assets.h"


/**
 * @brief Provides streamed access to assets included in a project
 * 
 * CGF's build tool compiles a project's runtime dependencies into a CGFB (C++ Game Framework Binary).
 * The AssetLibrary class streams the compiled CGFB and provides an interface to convert arbitrary
 * asset data into an instance of an asset. The asset data pertaining to a named asset is loaded
 * via AssetDataLoader upon a corresponding call to Get().
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
	SharedPtr<AssetType> Load(std::string assetName)
	{
		throw "Asset type not recognized";
	}


private:
	btools::AssetDataLoader m_AssetDataLoader;
	char* m_AssetData;
	int m_AssetDataSize;
};


template<>
inline SharedPtr<Material> AssetLibrary::Load(std::string materialName)
{
	char* rawSource;
	int rawSourceLength;
	bool success = m_AssetDataLoader.Load(materialName.c_str(), &rawSource, &rawSourceLength);

	CGF_ASSERT(success, "Failed to load " + materialName);

	std::string source (rawSource, rawSourceLength);

	auto fs = std::make_shared<Shader>(materialName, source, SHADER_TYPE_PIXEL);
	auto vs = std::make_shared<Shader>(materialName, source, SHADER_TYPE_VERTEX);

	return SharedPtr<Material>::CreateTraced(materialName + "_Material", vs, fs);
}


template<>
inline SharedPtr<std::string> AssetLibrary::Load(std::string materialName)
{
	char* rawSource;
	int rawSourceLength;
	bool success = m_AssetDataLoader.Load(materialName.c_str(), &rawSource, &rawSourceLength);

	CGF_ASSERT(success, "Failed to load " + materialName);

	std::string source (rawSource, rawSourceLength);
	return SharedPtr<std::string>::CreateTraced(materialName + "_Source", source);
}
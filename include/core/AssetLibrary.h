#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>


/**
 * @brief Provides access to assets included in a project
 **/
class AssetLibrary
{
public:
	AssetLibrary();

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



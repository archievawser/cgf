#include <string>
#include <vector>
#include <type_traits>
#include <iostream>
#include <unordered_map>

#include "pugixml.hpp"

#include "AssetTypes.h"
#include "cgfb.h"

#define LOG(x) std::cout << x << std::endl;


template <typename T>
void GetAssetListOfType(std::vector<T>* out, pugi::xml_document& doc)
{
	static_assert(std::is_base_of<BaseAsset<T>, T>::value);

	*out = std::vector<T>();

	for (pugi::xml_node& node : doc.child("Assets").children(T::RootXmlNodeName))
	{
		T asset;
		asset.ConsumeXml(node);

		out->push_back(asset);
	}
}


std::string GetFileDirectory(std::string filePath)
{
	int fileNameSize = 0;
	
	for(; filePath[filePath.size() - 1 - fileNameSize] != '/'; fileNameSize++);

	return filePath.substr(0, filePath.size() - fileNameSize);
}


std::string GetAbsoluteFilePath(const char* pathRelativeToContentDir)
{
	return "";
}


#define CGFB_WRITE
#define CGFB_READ


int main(int argc, char* argv[])
{
	char* contentFilePath = argv[1];
	char* contentBinaryOutputPath = argv[2];

	pugi::xml_document doc;
	doc.load_file(contentFilePath);

	std::unordered_map<std::string, int> assetNameToDataLocation;
	std::vector<char> assetData;
	
	std::vector<MaterialAsset> matList;
	GetAssetListOfType<MaterialAsset>(&matList, doc);

	{
		CgfbWriter hi (contentBinaryOutputPath);

		for(MaterialAsset& v : matList)
		{
			assetNameToDataLocation[v.Name] = assetData.size();

			char* materialData = nullptr;
			int materialDataSize = 0;

			v.ToBytes(&materialData, &materialDataSize);
			
			assetData.insert(assetData.end(), materialData, materialData + materialDataSize);
		}

		hi.Write(assetNameToDataLocation);
		hi.Write(assetData.data(), assetData.size());
	}

	{
		CgfbReader hi(contentBinaryOutputPath);

		std::unordered_map<std::string, int> e;
		hi.Read(&e);

		std::cout << "----------------" << std::endl;
		for (const auto &[k, v] : e)
		{
			std::cout << k << ": " << v << std::endl;
		}
		std::cout << "----------------" << std::endl;
	}
}
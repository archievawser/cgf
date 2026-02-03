#include <string>
#include <vector>
#include <type_traits>
#include <iostream>
#include <unordered_map>

#include "pugixml.hpp"

#include "buildtool/Assets.h"
#include "buildtool/AssetTypes.h"
#include "cgfb/CGFB.h"

#define LOG(x) std::cout << x << std::endl;


using namespace cgfb;
using namespace btools;


template <typename T>
void GetAssetListOfType(std::vector<T>* out, pugi::xml_document& doc)
{
	static_assert(std::is_base_of_v<BaseAsset, T>);

	*out = std::vector<T>();

	for (pugi::xml_node &node : doc.child("Assets").children(T().GetXmlRootNodeName()))
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
	LOG("Compiling project into binary");
	LOG("Project file: " + std::string(argv[1]));
	LOG("Binary file: " + std::string(argv[2]));

	char* contentFilePath = argv[1];
	char* contentBinaryOutputPath = argv[2];

	pugi::xml_document doc;
	doc.load_file(contentFilePath);

	std::unordered_map<std::string, int> assetNameToDataLocation;
	
	LOG("- Loading assets -");

	// Materials

	CgfbFileWriter outWriter (contentBinaryOutputPath);
	CgfbMemoryWriter dataWriter;
	
	std::vector<MaterialAsset> matList;
	GetAssetListOfType<MaterialAsset>(&matList, doc);
	for (MaterialAsset& v : matList)
	{
		LOG(v.Name);
		assetNameToDataLocation[v.Name] = dataWriter.GetBuffer().size();

		char* materialData = nullptr;
		int materialDataSize = 0;

		ReadFileContents(v.ShaderPath.c_str(), &materialData, &materialDataSize);

		dataWriter.Write(materialDataSize);
		dataWriter.Write(materialData, materialDataSize);
	}
	
	std::vector<MeshAsset> meshList;
	GetAssetListOfType<MeshAsset>(&meshList, doc);
	for (MeshAsset& v : meshList)
	{
		LOG(v.Name);
		assetNameToDataLocation[v.Name] = dataWriter.GetBuffer().size();

		char* meshData = nullptr;
		int meshDataSize = 0;

		ReadFileContents(v.Path.c_str(), &meshData, &meshDataSize);

		dataWriter.Write(meshDataSize);
		dataWriter.Write(meshData, meshDataSize);
	}

	LOG("------------------");

	outWriter.Write(assetNameToDataLocation);
	outWriter.Write((char*)dataWriter.GetBuffer().data(), dataWriter.GetBuffer().size());

	LOG("Project binary compiled!")
}
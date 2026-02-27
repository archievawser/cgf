#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <unordered_map>

#include "pugixml.hpp"

#include "cgfb/CGFB.h"
#include "stb/stb_image.h"
#include "buildtool/Assets.h"
#include "buildtool/AssetTypes.h"

#define LOG(x) std::cout << (x) << std::endl;

#define CGFB_WRITE
#define CGFB_READ

#define RUN_IN_DEBUG


using namespace cgfb;
using namespace btools;


#define DEV_IN "D:\\Dev\\C++\\cgf\\samples\\dev\\Project.xml"
#define DEV_OUT "D:\\Dev\\C++\\cgf\\hi.cgfb"


enum class AssetType
{
	Material,
	Mesh,
	Texture,

	NumTypes
};


template<AssetType AssetT>
void CompileAssetType(CgfbFileWriter& out, pugi::xml_document& descriptor)
{
	LOG("No compiler implemented for asset type " + std::to_string((int)AssetT));
}


/**
 * @brief Compiles material info into a CGFB asset file
 */
template<>
void CompileAssetType<AssetType::Material>(CgfbFileWriter& out, pugi::xml_document& document)
{
	LOG("Compiling materials...");

	for(auto& v : document.child("Assets").children("Material"))
	{
		std::string name = v.child_value("Name");
		std::string domain = v.child_value("Domain");
		std::string path = v.child_value("ShaderFile");

		char* source;
		int sourceLen;
		ReadFileContents(path.c_str(), &source, &sourceLen);

		out.StartBlock(name);
		out.Write(domain);
		out.Write(sourceLen);
		out.Write(source, sourceLen);
	}
}


/**
 * @brief Compiles mesh info into a CGFB asset file
 */
template<>
void CompileAssetType<AssetType::Mesh>(CgfbFileWriter& out, pugi::xml_document& document)
{
	LOG("Compiling meshes...");

	for(auto& v : document.child("Assets").children("Mesh"))
	{
		std::string name = v.child_value("Name");
		std::string path = v.child_value("File");

		char* source;
		int sourceLen;
		ReadFileContents(path.c_str(), &source, &sourceLen);

		out.StartBlock(name);
		out.Write(sourceLen);
		out.Write(source, sourceLen);
	}
}


/**
 * @brief Compiles texture info into a CGFB asset file
 */
template<>
void CompileAssetType<AssetType::Texture>(CgfbFileWriter& out, pugi::xml_document& document)
{
	LOG("Compiling textures...");

	for(auto& v : document.child("Assets").children("Texture")) 
	{
		std::string name = v.child_value("Name");
		std::string path = v.child_value("File");

		int x, y, channels;
		unsigned char* data = stbi_load(path.c_str(), &x, &y, &channels, 4);

		out.StartBlock(name);
		out.Write("Abcdefg");
		out.Write(x);
		out.Write(y);
		out.Write((char*)data, x * y * 4);
	}
}


template<size_t... TypeIndices>
void CompileAssetTypesFromTypeIndices(std::index_sequence<TypeIndices...> indices, CgfbFileWriter& out, pugi::xml_document& descriptor)
{
	( 
		CompileAssetType<(AssetType)TypeIndices>(out, descriptor), 
		... 
	);
}


void CompileAssets(CgfbFileWriter& out, pugi::xml_document& descriptor)
{
	CompileAssetTypesFromTypeIndices(std::make_index_sequence<(size_t)AssetType::NumTypes>(), out, descriptor);
}


int main(int argc, char* argv[])
{
	const char* projectFile = argc == 0 ? DEV_IN : argv[1];
	const char* binaryFile = argc == 0 ? DEV_OUT : argv[2];

	LOG("Compiling from " + std::string(projectFile) + " to " + std::string(binaryFile));

	stbi_set_flip_vertically_on_load(true);

	CgfbFileWriter stream (binaryFile);

	pugi::xml_document document;
	document.load_file(projectFile);

	CompileAssets(stream, document);

	LOG("Compiled to " + std::string(binaryFile));
}
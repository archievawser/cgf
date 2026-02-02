#pragma once

#include <string>
#include <fstream>

#include "pugixml.hpp"
#include "utility.h"


template<typename T>
struct BaseAsset
{
	virtual void ConsumeXml(pugi::xml_node &xmlNode) = 0;
	virtual void ToBytes(char** buffer, int* size) = 0;

	static constexpr const char* RootXmlNodeName = "Unknown";
	std::string Name;
};


struct MaterialAsset : public BaseAsset<MaterialAsset>
{
	void ToBytes(char** buffer, int* size) override
	{
		ReadFileContents(ShaderPath.c_str(), buffer, size);
	}

	void ConsumeXml(pugi::xml_node& xmlNode) override
	{
		Name = xmlNode.child("Name").child_value();
		ShaderPath = xmlNode.child("ShaderPath").child_value();
	}
	
	static constexpr const char* RootXmlNodeName = "Material";
	std::string ShaderPath;
};
#pragma once

#include <string>
#include <fstream>

#include "pugixml.hpp"
#include "utility.h"


namespace btools
{

struct BaseAsset
{
	virtual void ConsumeXml(pugi::xml_node &xmlNode) = 0;
	virtual inline const char* GetXmlRootNodeName() const = 0;

	std::string Name;
};


struct MaterialAsset : public BaseAsset
{
	void ConsumeXml(pugi::xml_node& xmlNode) override
	{
		Name = xmlNode.child("Name").child_value();
		ShaderPath = xmlNode.child("ShaderFile").child_value();
	}

	inline const char* GetXmlRootNodeName() const override
	{
		return "Material";
	}
	
	std::string ShaderPath;
};


struct MeshAsset : public BaseAsset
{
	void ConsumeXml(pugi::xml_node& xmlNode) override
	{
		Name = xmlNode.child("Name").child_value();
		Path = xmlNode.child("File").child_value();
	}

	inline const char* GetXmlRootNodeName() const override
	{
		return "Mesh";
	}

	std::string Path;
};


struct TextureAsset : public BaseAsset
{
	void ConsumeXml(pugi::xml_node& xmlNode) override
	{
		Name = xmlNode.child("Name").child_value();
		Path = xmlNode.child("File").child_value();
	}

	inline const char* GetXmlRootNodeName() const override
	{
		return "Texture";
	}

	std::string Path;
};

}
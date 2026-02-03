#pragma once

#include <string>

#include "cgfb/CGFB.h"


namespace btools
{

class AssetDataLoader : public cgfb::CgfbFileReader
{
public:
	AssetDataLoader(const char* contentFile);

	bool Load(const char* name, char** buff, int* count);

private:
	int m_DataBlockStartLocation;
	std::unordered_map<std::string, int> m_DataLocations;
};

};
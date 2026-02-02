#pragma once

#include <string>

#include "cgfb/CGFB.h"


class AssetDataLoader : public CgfbFileReader
{
public:
	AssetDataLoader(const char* contentFile);

	void Load(const char* name, char** buff, int* count);

private:
	int m_DataBlockStartLocation;
	std::unordered_map<std::string, int> m_DataLocations;
};
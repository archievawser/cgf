#include "buildtool/Assets.h"

#include <cassert>


using namespace btools;


AssetDataLoader::AssetDataLoader(const char *contentFile)
	: CgfbFileReader(contentFile)
{
	Read(&m_DataLocations);
	m_DataBlockStartLocation = GetPosition();
}


bool AssetDataLoader::Load(const char* name, char** buff, int* count)
{
	if(m_DataLocations.find(name) == m_DataLocations.end())
	{
		return false;
	}

	SeekStreamPosition(m_DataBlockStartLocation + m_DataLocations[name], std::ios_base::beg);

	Read(count);
	Read(*buff = new char[*count], *count);

	return true;
}
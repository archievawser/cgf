#include "buildtool/Assets.h"


AssetDataLoader::AssetDataLoader(const char *contentFile)
	: CgfbFileReader(contentFile)
{
	Read(&m_DataLocations);
	m_DataBlockStartLocation = GetPosition();
}


void AssetDataLoader::Load(const char* name, char** buff, int* count)
{
	SeekStreamPosition(m_DataBlockStartLocation + m_DataLocations[name], std::ios_base::beg);

	Read(count);
	Read(*buff = new char[*count], *count);
}
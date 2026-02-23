#include "cgfb/CGFB.h"

#include <cassert>
#include <iostream>


using namespace cgfb;
#define LOG(x) std::cout << x << std::endl;


CgfbFileWriter::CgfbFileWriter(const char *filePath)
	: m_File(filePath, std::ios_base::binary)
{

}


void CgfbFileWriter::WriteToStream(char* data, int count)
{
	assert(m_WithinBlock);

	m_DataStream.Write(data, count);
}


CgfbFileReader::CgfbFileReader(const char *filePath)
	: m_File(filePath, std::ios_base::binary)
{
	assert(m_File.good());

	CgfbReader::Read(&m_BlockData);
	m_BlockDataOffset = GetPosition();
}


void CgfbFileReader::ReadFromStream(char *data, int count)
{
	m_File.read(data, count);
}


void CgfbFileReader::SeekStreamPosition(int position, std::ios_base::seekdir way)
{
	m_File.seekg(position, way);
}


void CgfbFileReader::ReadBlock(const char *blockName, char **buffer, int *bufferSize)
{
	BlockInfo& info = m_BlockData[blockName];

	SeekStreamPosition(m_BlockDataOffset + info.StartIndex, std::ios_base::beg);

	LOG("Block data offset, Start index, Their sum, Block size");
	LOG(m_BlockDataOffset);
	LOG(info.StartIndex);
	LOG(m_BlockDataOffset + info.StartIndex);
	LOG(info.Size);

	*buffer = new char[info.Size];
	*bufferSize = info.Size;

	Read(*buffer, *bufferSize);
}


void CgfbMemoryWriter::WriteToStream(char *data, int count)
{
	m_Buffer.insert(m_Buffer.end(), data, data + count);
}


CgfbMemoryReader::CgfbMemoryReader(char* buffer, int bufferSize)
{
	m_Buffer = new char[bufferSize];
}


CgfbMemoryReader::CgfbMemoryReader(char*&& buffer)
	: m_Buffer(buffer)
{

}


CgfbMemoryReader::~CgfbMemoryReader()
{
	delete m_Buffer;
}


void CgfbMemoryReader::ReadFromStream(char *data, int count)
{
	static int pos = 0;

	std::memcpy(data, m_Buffer + pos, count);

	pos += count; 
}
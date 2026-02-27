#include "cgfb/CGFB.h"

#include <cassert>
#include <iostream>


using namespace cgfb;
#define LOG(x) std::cout << x << std::endl;


CgfbFileWriter::CgfbFileWriter(const char *filePath)
	: m_File(filePath, std::ios_base::binary)
{

}


cgfb::CgfbFileWriter::~CgfbFileWriter()
{
	Flush();
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


void CgfbFileReader::ReadBlock(std::string blockName, CgfbBlock& out)
{
	BlockInfo& info = m_BlockData[blockName];

	SeekStreamPosition(m_BlockDataOffset + info.StartIndex, std::ios_base::beg);

	LOG("Block data offset, Start index, Their sum, Block size");
	LOG(m_BlockDataOffset);
	LOG(info.StartIndex);
	LOG(m_BlockDataOffset + info.StartIndex);
	LOG(info.Size);
	
	out.Data = new char[info.Size];
	out.Count = info.Size;
	Read(out.Data, out.Count);
}


void CgfbMemoryWriter::WriteToStream(char *data, int count)
{
	m_Buffer.insert(m_Buffer.end(), data, data + count);
}


CgfbMemoryReader::CgfbMemoryReader(char* buffer, int bufferSize)
{
	m_Buffer = new char[bufferSize];
}


CgfbMemoryReader::CgfbMemoryReader(CgfbBlock &&block)
	: m_Buffer(block.Data)
{
	block.Data = nullptr;
	block.Count = 0;
}


CgfbMemoryReader::~CgfbMemoryReader()
{
	delete m_Buffer;
}


void CgfbMemoryReader::ReadFromStream(char *data, int count)
{
	std::memcpy(data, m_Buffer + m_Position, count);

	m_Position += count;
}


cgfb::CgfbBlock::CgfbBlock(char *data, size_t count)
	: Data(data), Count(count)
{

}


cgfb::CgfbBlock::~CgfbBlock()
{
	delete[] Data;
}

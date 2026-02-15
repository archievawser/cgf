#include "cgfb/CGFB.h"

#include <cassert>


using namespace cgfb;


CgfbFileWriter::CgfbFileWriter(const char *filePath)
	: m_File(filePath, std::ios_base::binary)
{

}


void CgfbFileWriter::WriteToStream(char* data, int count)
{
	m_File.write(data, count);
}


CgfbFileReader::CgfbFileReader(const char *filePath)
	: m_File(filePath, std::ios_base::binary)
{
	assert(m_File.good());
}


void CgfbFileReader::ReadFromStream(char *data, int count)
{
	m_File.read(data, count);
}


void CgfbFileReader::SeekStreamPosition(int position, std::ios_base::seekdir way)
{
	m_File.seekg(position, way);
}


void CgfbMemoryWriter::WriteToStream(char *data, int count)
{
	m_Buffer.insert(m_Buffer.end(), data, data + count);
}


CgfbMemoryReader::CgfbMemoryReader(char* buffer)
	: m_Buffer(buffer)
{

}


void CgfbMemoryReader::ReadFromStream(char *data, int count)
{
	static int pos = 0;

	std::memcpy(data, m_Buffer + pos, count);

	pos += count; 
}
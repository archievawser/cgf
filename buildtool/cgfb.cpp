#include "cgfb.h"


CgfbWriter::CgfbWriter(const char *filePath)
	: m_File(filePath, std::ios_base::binary)
{

}


CgfbWriter::~CgfbWriter()
{
	m_File.close();
}


CgfbReader::CgfbReader(const char *filePath)
	: m_File(filePath)
{

}


CgfbReader::~CgfbReader()
{
	m_File.close();
}

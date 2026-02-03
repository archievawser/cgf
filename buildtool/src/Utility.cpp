#include "buildtool/Utility.h"

#include <iostream>
#include <fstream>


void btools::ReadFileContents(const char* filePath, char** buff, int* buffSize)
{
	static char* fileStreamBuffer = new char[FILE_READ_CHUNK_SIZE];
	std::ifstream file (filePath, std::ios_base::binary);

	char* readBuffer = nullptr;
	int readBufferSize = 0;

	do
	{
		file.read(fileStreamBuffer, FILE_READ_CHUNK_SIZE);
		int bytesRead = file.gcount();
	
		int resizedReadBufferSize = readBufferSize + bytesRead;
		char* resizedReadBuffer = new char[resizedReadBufferSize];

		std::memcpy(resizedReadBuffer, readBuffer, readBufferSize); // copy bytes from old buffer to new
		std::memcpy(resizedReadBuffer + readBufferSize, fileStreamBuffer, bytesRead); // append bytes read onto resized buffer

		readBuffer = resizedReadBuffer;
		readBufferSize = resizedReadBufferSize;
	} 
	while (!file.eof());

	*buff = readBuffer;
	*buffSize = readBufferSize;
}
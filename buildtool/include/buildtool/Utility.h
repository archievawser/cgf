#pragma once

#define FILE_READ_CHUNK_SIZE (1 << 18)


namespace btools
{

void ReadFileContents(const char* filePath, char** buff, int* buffSize);
	
}
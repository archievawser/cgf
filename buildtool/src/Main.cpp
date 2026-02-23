#include <string>
#include <vector>
#include <type_traits>
#include <iostream>
#include <unordered_map>

#include "pugixml.hpp"

#include "buildtool/Assets.h"
#include "buildtool/AssetTypes.h"
#include "cgfb/CGFB.h"
#include "stb/stb_image.h"

#define LOG(x) std::cout << x << std::endl;

#define CGFB_WRITE
#define CGFB_READ

#define RUN_IN_DEBUG


using namespace cgfb;
using namespace btools;


#define TEST_FILE "D:\\Dev\\C++\\cgf\\hi.cgfb"


int main(int argc, char* argv[])
{
	{
		CgfbFileWriter file (TEST_FILE);

		file.StartBlock("Initial block");
		file.Write("Bye!");
		file.StartBlock("Reverse block");
		file.Write("Hello!");
		file.Flush();
	}

	{
		CgfbFileReader reader (TEST_FILE);
		
		char* buff;
		int size;
		reader.ReadBlock("Reverse block", &buff, &size);

		std::string e (buff, size);
		LOG(e);
	}
}
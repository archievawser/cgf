cmake_minimum_required(VERSION 3.6)

include(FetchContent)

project(buildtool)

add_library(buildtool 
	"src/Assets.cpp" 
	"src/cgfb/CGFB.cpp" 
	"src/Utility.cpp")

FetchContent_Declare(
	pugixml
	GIT_REPOSITORY https://github.com/zeux/pugixml.git
)

FetchContent_MakeAvailable(pugixml)

target_include_directories(buildtool PUBLIC include PUBLIC ${pugixml_SOURCE_DIR}/src)
target_link_libraries(buildtool PUBLIC pugixml)

add_executable(cgfb_compiler
	"src/Main.cpp")

target_link_libraries(cgfb_compiler PUBLIC buildtool)

function(compile_cgfb_on_build TARGET CONTENT_FILE_PATH BINARY_OUTPUT_FILE_PATH)
	add_custom_command(
		TARGET ${TARGET} POST_BUILD

		COMMAND $<TARGET_FILE:cgfb_compiler> ARGS ${CONTENT_FILE_PATH} ${BINARY_OUTPUT_FILE_PATH}
	)
endfunction()
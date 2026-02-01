cmake_minimum_required(VERSION 3.6)

include(FetchContent)

project(buildtool)

add_executable(buildtool "Main.cpp" "cgfb.cpp" "utility.cpp")

FetchContent_Declare(
	pugixml
	GIT_REPOSITORY https://github.com/zeux/pugixml.git
)

FetchContent_MakeAvailable(pugixml)

target_include_directories(buildtool PUBLIC ${pugixml_SOURCE_DIR}/src)
target_link_libraries(buildtool PUBLIC pugixml)

function(compile_cgfb_on_build TARGET CONTENT_FILE_PATH BINARY_OUTPUT_FILE_PATH)
	add_custom_command(
		TARGET ${TARGET} POST_BUILD

		COMMAND $<TARGET_FILE:buildtool> ARGS ${CONTENT_FILE_PATH} ${BINARY_OUTPUT_FILE_PATH}
	)
endfunction()
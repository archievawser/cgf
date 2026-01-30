set(SOURCE
	"Main.cpp"
)

add_executable(dev_build ${SOURCE})

target_link_libraries(dev_build PUBLIC cgf)

copy_required_dlls(dev_build)

add_custom_command(
	TARGET dev_build POST_BUILD

	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/prototype.hlsl
	$<TARGET_FILE_DIR:dev_build>/shaders/prototype.hlsl

	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/prototype.hlsl
	$<TARGET_FILE_DIR:dev_build>/shaders/prototype.hlsl
)
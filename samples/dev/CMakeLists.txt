set(SOURCE
	"Main.cpp"
)

add_executable(dev_build ${SOURCE})

copy_required_dlls(dev_build)

target_link_libraries(dev_build PUBLIC cgf)

compile_cgfb_on_build(
	dev_build
	${CMAKE_CURRENT_SOURCE_DIR}/Project.xml
	$<TARGET_FILE_DIR:dev_build>/Content.cgfb
)
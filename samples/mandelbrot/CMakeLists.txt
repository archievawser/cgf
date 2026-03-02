set(SOURCE
	"Main.cpp"
)

add_executable(mandelbrot ${SOURCE})

copy_required_dlls(mandelbrot)

target_link_libraries(mandelbrot PUBLIC cgf)

compile_cgfb_on_build(
	mandelbrot
	${CMAKE_CURRENT_SOURCE_DIR}/Project.xml
	$<TARGET_FILE_DIR:mandelbrot>/Content.cgfb
)
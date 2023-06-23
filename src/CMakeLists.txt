add_compile_options(
	-Werror
	-Wall
	-Wextra
	-Wpedantic
	-Wstrict-aliasing=3
	-Wwrite-strings
	-Wvla
	-Wcast-align=strict
	-Wstrict-prototypes
	-Wstringop-overflow=4
	-Wno-logical-op-parentheses

	$<$<CONFIG:RELEASE>:-O2>
	$<$<CONFIG:DEBUG>:-O0>
	$<$<CONFIG:DEBUG>:-ggdb>
)

add_compile_definitions(
	$<$<CONFIG:RELEASE>:NDEBUG>
)


set(TARGET ${CMAKE_PROJECT_NAME})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${TARGET} ${SDL2_INCLUDE_DIRS})
include_directories(${TARGET} ${SDL2_IMAGE_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)
set(SOURCES ${SOURCES})
add_executable(${TARGET}_run ${SOURCES})
add_library(${TARGET}_lib STATIC ${SOURCES})
target_link_libraries(${TARGET}_run
		${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})

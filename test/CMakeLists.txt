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
	-O2
)

set(TARGET ${CMAKE_PROJECT_NAME})
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)
set(SOURCES ${SOURCES})
add_executable(${TARGET}_test ${SOURCES})

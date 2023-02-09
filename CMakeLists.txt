cmake_minimum_required(VERSION 3.10)
project(nesemu2 
	VERSION 1.0
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)


add_compile_options(
	-Wall 
	-Wextra 
	-Wno-unused-parameter 
	-Wfloat-equal
	-Wundef
	-Wshadow
	-Wpointer-arith
	-Wcast-align
	-Wstrict-prototypes
	-Wstrict-overflow=5
	-Wwrite-strings
	-Waggregate-return
	-Wcast-equal
	-Wswitch-default
	-Wswitch-enum
	-Wconversion
	-Wunreachable-code

	-fno-stack-protector
	-finstrument-functions
	--coverage
	-fsanitize={address, thread, undefined}
	-pedantic

	$<$<CONFIG:RELEASE>:-O2>
	$<$<CONFIG:DEBUG>:-O0>
	$<$<CONFIG:DEBUG>:-ggdb3>
	)

add_compile_definitions(
	$<$<CONFIG:RELEASE>:NDEBUG>
	)


find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(nesemu2 ${SDL2_INCLUDE_DIRS})
include_directories(nesemu2 ${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(nesemu2 ${SDL2_TTF_INCLUDE_DIRS})

add_library(nesemu2_lib src/cpu.c src/opcode.c src/logger.c)

add_executable(nesemu2 src/main.c)
target_link_libraries(nesemu2
		${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES}
		nesemu2_lib)

# TESTING SECTION

include(ExternalProject)
ExternalProject_Add(googletest
    PREFIX "${CMAKE_BINARY_DIR}/lib"
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "main"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/lib/installed
)

set_target_properties(googletest PROPERTIES EXCLUDE_FROM_ALL TRUE)

include(GNUInstallDirs)

link_directories(${CMAKE_BINARY_DIR}/lib/installed/${CMAKE_INSTALL_LIBDIR})
include_directories(${CMAKE_BINARY_DIR}/lib/installed/${CMAKE_INSTALL_INCLUDEDIR})

find_package(Threads REQUIRED)

add_executable(nesemu2_test test/opcode_test.cc)
target_link_libraries(nesemu2_test nesemu2_lib gtest Threads::Threads)

add_dependencies(nesemu2_test googletest)
set_target_properties(nesemu2_test PROPERTIES EXCLUDE_FROM_ALL TRUE)

cmake_minimum_required(VERSION 3.10)
project(nesemu2 
	VERSION 1.0
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
	-Wall -Werror -Wno-error=maybe-uninitialized
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

add_executable(nesemu2 
	src/main.c
	src/cpu.c
	src/logger.c
	src/opcode.c)

target_link_libraries(nesemu2 ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
	nesemu2_test
	test/opcode_test.cc
	)

target_link_libraries(
	nesemu2_test
	GTest::gtest_main
	)

include(GoogleTest)
gtest_discover_tests(nesemu2_test)





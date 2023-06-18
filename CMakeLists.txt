cmake_minimum_required(VERSION 3.10)
project(nesemu2 
	VERSION 1.0
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "") 
	set(CMAKE_BUILD_TYPE Debug)
endif()

add_subdirectory(src)
add_subdirectory(test)

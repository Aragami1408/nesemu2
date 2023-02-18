add_compile_options(
    -Wall
    -Wextra
)

set(TARGET ${CMAKE_PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cc)
set(SOURCES ${TEST_SOURCES})
add_executable(${TARGET} ${TEST_SOURCES})
add_test(NAME ${TARGET} COMMAND ${TARGET})
target_link_libraries(${TARGET} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest)
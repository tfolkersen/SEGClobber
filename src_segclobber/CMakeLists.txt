file(GLOB segclobber_SRC *.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(segclobber ${segclobber_SRC})

target_link_libraries(
    segclobber
    PRIVATE segclobber_common
    PUBLIC cgt_test_lib
)

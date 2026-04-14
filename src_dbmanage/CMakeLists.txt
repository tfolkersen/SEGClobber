file(GLOB segclobber_dbmanage_SRC *.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(dbmanage ${segclobber_dbmanage_SRC})

target_link_libraries(
    dbmanage 
    segclobber_common
)

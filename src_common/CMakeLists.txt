file(GLOB segclobber_common_SRC *.cpp)

add_library(segclobber_common OBJECT ${segclobber_common_SRC})

target_include_directories(
    segclobber_common
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/"
)

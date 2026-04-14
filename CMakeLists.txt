cmake_minimum_required(VERSION 4.2)
project(segclobber-cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

############################## Configurable options
set(DEBUG_LEVEL "1" CACHE STRING
    "Debug level 0/1/2"
)

set(ASAN "0" CACHE STRING
    "Compile with AddressSanitizer? 0/1"
)

set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data" CACHE PATH
    "Path to directory containing database/test data files"
)

############################## Compute CXX flags
# Convert DATA_DIR to an absolute path
cmake_path(ABSOLUTE_PATH DATA_DIR BASE_DIRECTORY ${DATA_DIR} NORMALIZE)

# Note the confusing escapes (data directory is defined as a string with quotes)
set(SEGCLOBBER_FLAGS_BASE "-Wall -O3 -DSEGCLOBBER_DATA_DIR=\"\\\"${DATA_DIR}\\\"\"")

if(DEBUG_LEVEL STREQUAL "0")
    set(SEGCLOBBER_FLAGS_DEBUG "-DNDEBUG")
elseif(DEBUG_LEVEL STREQUAL "1")
    set(SEGCLOBBER_FLAGS_DEBUG "-g")
elseif(DEBUG_LEVEL STREQUAL "2")
    set(SEGCLOBBER_FLAGS_DEBUG "-g -D_GLIBCXX_DEBUG")
else()
    message(FATAL_ERROR "Invalid value for DEBUG_LEVEL: \"${DEBUG_LEVEL}\"")
endif()

if(ASAN STREQUAL "0")
    set(SEGCLOBBER_FLAGS_ASAN "")
elseif(ASAN STREQUAL "1")
    set(SEGCLOBBER_FLAGS_ASAN "-g -fno-omit-frame-pointer -fsanitize=address")
else()
    message(FATAL_ERROR "Invalid value for ASAN: \"${ASAN}\"")
endif()

############################## Set CXX flags
set(SEGCLOBBER_CXX_FLAGS "\
${SEGCLOBBER_FLAGS_BASE} \
${SEGCLOBBER_FLAGS_DEBUG} \
${SEGCLOBBER_FLAGS_ASAN}"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${SEGCLOBBER_CXX_FLAGS}")

################################################## Build executables
add_subdirectory( cgt_testing_library )
add_subdirectory( src_common )
add_subdirectory( src_segclobber )
add_subdirectory( src_dbmanage )

################################################## Add script targets
set(SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

add_custom_target(test
    COMMAND bash -c "\
time python3 ${SCRIPT_DIR}/_runtests.py \
$<TARGET_FILE:segclobber> ${DATA_DIR}"

    DEPENDS segclobber
    VERBATIM
    USES_TERMINAL
)

add_custom_target(ftest
    COMMAND bash -c "\
time python3 ${SCRIPT_DIR}/_runtests.py \
$<TARGET_FILE:segclobber> ${DATA_DIR} --persist"

    DEPENDS segclobber
    VERBATIM
    USES_TERMINAL
)

add_custom_target(db
    COMMAND bash -c "\
rm -f ${DATA_DIR}/database3.bin ${DATA_DIR}/database3_alt.bin \
&& touch ${DATA_DIR}/database3.bin ${DATA_DIR}/database3_alt.bin \
&& time $<TARGET_FILE:dbmanage>"

    DEPENDS dbmanage
    VERBATIM
    USES_TERMINAL
)


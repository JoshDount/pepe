# I/O module - Data loaders and parsers
# Core I/O implementation with concrete file handling

# Create I/O source files directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

# I/O library with actual implementation
add_library(rts_io STATIC
    # Core I/O sources will be added here
    ${CMAKE_CURRENT_SOURCE_DIR}/io_placeholder.cpp
)

target_include_directories(rts_io PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rts_io PUBLIC rts_core)

# Set C++20 standard for I/O
target_compile_features(rts_io PUBLIC cxx_std_20)

# I/O implementation directories (future expansion)
# add_subdirectory(gtfs)
# add_subdirectory(osm)
# add_subdirectory(external)
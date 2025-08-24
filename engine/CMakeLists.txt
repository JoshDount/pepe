# Engine module - Routing and traffic simulation
# Core engine implementation with concrete algorithms

# Create engine source files directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Engine library with actual implementation
add_library(rts_engine STATIC
    # Core engine sources will be added here
    ${CMAKE_CURRENT_SOURCE_DIR}/engine_placeholder.cpp
)

target_include_directories(rts_engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rts_engine PUBLIC rts_core rts_io)

# Set C++20 standard for engine
target_compile_features(rts_engine PUBLIC cxx_std_20)

# Engine implementation directories (future expansion)
# add_subdirectory(routing)
# add_subdirectory(traffic)
# add_subdirectory(index)
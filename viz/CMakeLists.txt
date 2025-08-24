# Visualization module
add_library(rts_viz INTERFACE)

target_include_directories(rts_viz INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rts_viz INTERFACE rts_core)

# Add visualization headers
target_sources(rts_viz INTERFACE
    VisualizationFramework.hpp
    AlgorithmVisualizer.hpp
    DataStructureVisualizer.hpp
    SimulationVisualizer.hpp
)

# Placeholder for future visualization implementations
# add_subdirectory(render)
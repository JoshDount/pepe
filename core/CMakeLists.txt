# Core library - Data structures and algorithms
add_library(rts_core STATIC
    graph/Node.cpp
)

# Include directories
target_include_directories(rts_core INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add all headers
target_sources(rts_core PUBLIC
    ErrorHandling.hpp
    containers/MinHeap.hpp
    containers/HashMap.hpp
    containers/LinkedList.hpp
    containers/Queue.hpp
    containers/Stack.hpp
    containers/LRUCache.hpp
    trees/AVLTree.hpp
    graph/Node.hpp
    graph/Edge.hpp
    graph/WeightedGraph.hpp
    search/BFS.hpp
    search/DFS.hpp
    search/Dijkstra.hpp
    search/AStar.hpp
    search/MST.hpp
    algorithms/InternalSort.hpp
    algorithms/ExternalSort.hpp
    simulation/EventQueue.hpp
    simulation/Events.hpp
    traffic/TrafficSimulation.hpp
    io/GTFSData.hpp
    io/GTFSParser.hpp
    io/GTFSConverter.hpp
)

# Set C++20 features
target_compile_features(rts_core INTERFACE cxx_std_20)
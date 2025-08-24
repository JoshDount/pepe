# CLI module
add_executable(route_planner
    main.cpp
)

target_link_libraries(route_planner 
    rts_core 
    rts_io 
    rts_engine 
    rts_viz 
    rts_bench
)

# Placeholder main.cpp will be created later
add_executable(test_permutation test_permutation.cpp)
add_executable(test_group test_group.cpp)
add_executable(test_graph test_graph.cpp)

target_link_libraries(test_permutation
    contrib_catch_main
    source
)

target_link_libraries(test_group
    contrib_catch_main
    source
)

target_link_libraries(test_graph
    contrib_catch_main
    source
)

add_library(source STATIC
    ${PROJECT_SOURCE_DIR}/src/Permutation.cpp
    ${PROJECT_SOURCE_DIR}/src/Group.cpp
    ${PROJECT_SOURCE_DIR}/src/Certificate.cpp    
    ${PROJECT_SOURCE_DIR}/src/Structure.cpp
    ${PROJECT_SOURCE_DIR}/src/Graph.cpp
)


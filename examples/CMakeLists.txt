add_executable(gen_trees Trees.cpp)

target_link_libraries(gen_trees
    source
)

add_executable(one_vertex RamseyExtension.cpp)

target_link_libraries(one_vertex
    source
)

add_executable(gluing RamseyGluing.cpp)

target_link_libraries(gluing
    source
)

add_executable(turan Turan.cpp)

target_link_libraries(turan
    source
)

include_directories(
    PRIVATE ${PROJECT_SOURCE_DIR}/inc
)

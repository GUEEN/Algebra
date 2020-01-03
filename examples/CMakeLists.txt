add_executable(gen_trees Trees.cpp)

target_link_libraries(gen_trees
    source
)

include_directories(
    PRIVATE ${PROJECT_SOURCE_DIR}/inc
)

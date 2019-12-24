add_executable(test_permutation test_permutation.cpp)

add_library(source STATIC
    ${PROJECT_SOURCE_DIR}/src/Permutation.cpp
)

target_link_libraries(test_permutation
    contrib_catch_main
    source
)

include_directories(
    PRIVATE ${PROJECT_SOURCE_DIR}/inc
)

add_executable(generate Generate.cpp)
add_executable(one_vertex RamseyExtension.cpp)
add_executable(gluing RamseyGluing.cpp)
add_executable(turan Turan.cpp)
add_executable(borsuk Borsuk.cpp)
add_executable(reg Borsuk_regular.cpp)
add_executable(stats Stats.cpp)

target_link_libraries(generate
    source
)

target_link_libraries(one_vertex
    source
)

target_link_libraries(gluing
    source
)

target_link_libraries(turan
    source
)

target_link_libraries(borsuk
    source
)

target_link_libraries(reg
    source
)

target_link_libraries(stats
    source
)

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
)

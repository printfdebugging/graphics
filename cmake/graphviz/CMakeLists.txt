cmake_minimum_required(VERSION 4.0)

set(GRAPHVIZ_IGNORE_TARGETS OFF)

# generate graphviz files
# TODO: create images from the .dot files
add_custom_target(graphviz
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/graphviz
        COMMAND ${CMAKE_COMMAND} --graphviz=${CMAKE_BINARY_DIR}/graphviz/dependencies.dot ${CMAKE_SOURCE_DIR}
        COMMENT "Generate dependency graphs"
)

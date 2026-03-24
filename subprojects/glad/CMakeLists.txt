cmake_minimum_required(VERSION 4.0)
include(GNUInstallDirs)
project(glad)

add_library(glad
    src/glad.c
)

target_include_directories(glad
    PUBLIC include
)

target_compile_options(glad
    PRIVATE -fPIC
)

install(
    DIRECTORY 
        include/glad 
        include/KHR
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    TARGETS glad
    EXPORT  glad
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/glad.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/glad.pc @ONLY
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/glad.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

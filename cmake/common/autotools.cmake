# https://github.com/scivision/cmakeutils/blob/main/cmake/FindAutotools.cmake

find_program(AUTOCONF_EXECUTABLE
    NAMES autoconf
    DOC "Autoconf"
)

if(AUTOCONF_EXECUTABLE)
    execute_process(COMMAND ${AUTOCONF_EXECUTABLE} --version
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE out
    )
    message(DEBUG "${out}")
    if(ret EQUAL 0)
        string(REGEX MATCH "autoconf .*([0-9]+\\.[0-9]+)" _m "${out}")
        set(AUTOCONF_VERSION "${CMAKE_MATCH_1}")
    endif()
endif()

find_program(AUTOMAKE_EXECUTABLE
    NAMES automake
    DOC "Automake"
)

find_program(LIBTOOL_EXECUTABLE
    NAMES glibtool libtool
    NAMES_PER_DIR
    DOC "libtool"
)

find_program(M4_EXECUTABLE
    NAMES gm4 m4
    NAMES_PER_DIR
    DOC "M4"
)

find_program(MAKE_EXECUTABLE
    NAMES gmake make ming32-make REQUIRED
    NAMES_PER_DIR
    DOC "GNU Make"
)

# find_program(BEAR_EXECUTABLE
#     NAMES bear REQUIRED
#     NAMES_PER_DIR
#     DOC "Bear"
# )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Autotools
    VERSION_VAR AUTOCONF_VERSION
    REQUIRED_VARS AUTOCONF_EXECUTABLE AUTOMAKE_EXECUTABLE MAKE_EXECUTABLE
)

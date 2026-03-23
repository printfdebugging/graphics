find_program(MESON_EXECUTABLE
    NAMES meson
    DOC "Meson"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Meson REQUIRED_VARS MESON_EXECUTABLE)

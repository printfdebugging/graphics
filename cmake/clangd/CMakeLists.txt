cmake_minimum_required(VERSION 4.0)

# GH: https:// github.com/clangd/clangd/issues/1621
if (EMSCRIPTEN)
   execute_process(
      COMMAND ${CMAKE_C_COMPILER} --cflags
      OUTPUT_VARIABLE CLANGD_FLAGS_TO_ADD
   )
else()
   set(CLANGD_FLAGS_TO_ADD "") # compile_commands.json is sufficient
endif()

separate_arguments(CLANGD_FLAGS_TO_ADD UNIX_COMMAND "${CLANGD_FLAGS_TO_ADD}")
list(JOIN CLANGD_FLAGS_TO_ADD ", " CLANGD_FLAGS_TO_ADD)
set(CLANGD_TEMPLATE ${CMAKE_CURRENT_LIST_DIR}/clangd.in)
configure_file(${CLANGD_TEMPLATE} "${CMAKE_SOURCE_DIR}/.clangd")

#!/bin/env bash

pkgconfig_path="$(pwd)/install/lib/pkgconfig"
application_source_dir="$(pwd)"
application_build_dir="$(pwd)/build"

bear_config="$(pwd)/bear.yml"
bear_output="$(pwd)/compile_commands.json"
bear_script="$(pwd)/install/bin/bear"
bear_cmd="${bear_script} --config ${bear_config} --output ${bear_output} --append --"

export PKG_CONFIG_PATH="${pkgconfig_path}"
${bear_cmd} cmake -S "${application_source_dir}" -B "${application_build_dir}"
${bear_cmd} cmake --build "${application_build_dir}" -j
